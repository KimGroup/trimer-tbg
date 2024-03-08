import numpy as np
import numexpr as ne
import scipy
from util import *

def next_pow_two(n):
    i = 1
    while i < n:
        i = i << 1
    return i

def autocorr_func_1d(x, norm=True):
    x = np.atleast_1d(x)
    if len(x.shape) != 1:
        raise ValueError("invalid dimensions for 1D autocorrelation function")
    n = next_pow_two(len(x))

    # Compute the FFT and then (from that) the auto-correlation function
    f = np.fft.fft(x - np.mean(x), n=2 * n)
    acf = np.fft.ifft(f * np.conjugate(f))[: len(x)].real
    acf /= 4 * n

    # Optionally normalize
    if norm and np.abs(acf[0]) > 1e-6:
        acf /= acf[0]

    return acf

# Automated windowing procedure following Sokal (1989)
def auto_window(taus, c):
    m = np.arange(len(taus)) < c * taus
    if np.any(m):
        return np.argmin(m)
    return len(taus) - 1


# Following the suggestion from Goodman & Weare (2010)
def autocorr_gw2010(y, c=5.0):
    f = autocorr_func_1d(np.mean(y, axis=0))
    taus = 2.0 * np.cumsum(f) - 1.0
    window = auto_window(taus, c)
    return taus[window]


def autocorr_new(y, c=5.0):
    f = np.zeros(y.shape[1])
    for yy in y:
        f += autocorr_func_1d(yy)
    f /= len(y)
    taus = 2.0 * np.cumsum(f) - 1.0
    window = auto_window(taus, c)
    return taus[window]


def optimize_histogram(x):
    max = len(x)
    bsize = 64
    while True:
        prevhist = np.histogram(x, bins=bsize)
        curmax = prevhist[0].max()
        if not curmax < max*0.95:
            break
        max = curmax
        bsize *= 2

    while True:
        bsize = int(bsize*0.9)
        curhist = np.histogram(x, bins=bsize)
        if curhist[0].max() > max*1.05:
            return prevhist
        prevhist = curhist


def get_runs(glob):
    runs = []
    for f in sorted(enum_files(glob), key=lambda x: x["t"]):
        e, k, s = read_histogram(f["fname"], skip=0)

        runs.append({
            "n": f["l"]**2//3 - f["r"]//3,
            "l": f["l"],
            "t": f["t"],
            "e": e,
            "mean_e": e.mean(),
            "std_e": e.std(),
            "tau_e": autocorr_gw2010(e[None, :10_000]),
            "k": k,
            "tau_k": autocorr_gw2010(k[None, :10_000]),
            "s": np.abs(s),
            "tau_s": autocorr_gw2010(s[None, :10_000]),
        })

        runs[-1]["g_inv"] = 1/(1+2*runs[-1]["tau_e"])
    return runs

def reweighted_observable(t, f, o, fs, runs, stride=1):
    """
    t: temperature
    f: (negative) free energy at temperature
    o: observable to reweight (lambda run: obs)

    fs: optimized free energy solutions
    runs: all runs
    """
    padded_fs = np.concatenate(([0], fs))

    total = 0
    for runi in runs:
        e = runi["e"][::stride]
        mean_e = runi["mean_e"]
        std_e = runi["std_e"]

        denom = np.zeros(len(e))
        for fj, runj in zip(padded_fs, runs):
            beta_diff = 1/runj['t'] - 1/t
            if beta_diff < 0:
                largest_term = -beta_diff * (mean_e + std_e)
            else:
                largest_term = -beta_diff * (mean_e - std_e)

            if largest_term - (fj - f) > 150:
                denom[:] = np.inf
                break
            elif largest_term - (fj - f) < -150:
                continue

            # denom += (len(runj["e"]) * runj["g_inv"]) * np.exp(-(1/runj["t"] - 1/t) * runi["e"] - (fj + f))
            denom += ne.evaluate(f"{len(runj['e'][::stride]) * runj['g_inv']} * exp(-({beta_diff}) * e - {fj - f})")

        if callable(o):
            coef = o(runi)
        else:
            coef = o

        total += np.sum(ne.evaluate(f"({runi['g_inv']} * coef) / denom"))

    return total

def fun_scalar(f, runs):
    if len(runs) != 2:
        raise ValueError()
    return reweighted_observable(runs[1]["t"], f, 1, [f], runs) - 1

def init_guess(runs):
    rough_f = np.array([r["e"].mean()/r["t"] for r in runs])
    rough_f2 = [0]
    for i in range(len(runs)-1):
        guess = rough_f[i+1] - rough_f[i]
        root = scipy.optimize.brentq(fun_scalar, a=guess-5000, b=guess+5000, args=runs[i:i+2])
        rough_f2.append(root + rough_f2[-1])
    return np.array(rough_f2)[1:]

def _del2(p0, p1, d):
    return p0 - np.square(p1 - p0) / d

def _relerr(actual, desired):
    return np.max(np.abs((actual - desired) / desired))

def iterate(fs, runs, stride=1):
    totals = []
    for f, run in zip(fs, runs[1:]):
        totals.append(f + np.log(reweighted_observable(run["t"], f, 1, fs, runs, stride=stride)))
    totals = np.array(totals)
    return totals, _relerr(totals, fs)

def iterate_del2(x0, runs):
    x1, _ = iterate(x0, runs)
    x2, _ = iterate(x1, runs)
    d = x2 - 2.0 * x1 + x0
    p = scipy._lib._util._lazywhere(d != 0, (x0, x1, d), f=_del2, fillvalue=x2)
    err = _relerr(p, x0)
    return p, err