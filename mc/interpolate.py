import sys
import json
import numpy as np
import mhr
import os

glob = sys.argv[1]
runs = mhr.get_runs(glob)
data = None

nsample = int(sys.argv[3])
order_param = sys.argv[4]

try:
    with open(sys.argv[2], "r") as f:
        pkl = json.load(f)
        data = pkl
except:
    pass

if data is None:
    data = {"f": mhr.init_guess(runs)}

    best_loss = 1
    while best_loss > 1e-6:
        for i in range(3):
            data["f"], err = mhr.iterate(data["f"], runs)
        data["f"] = data["f"].tolist()
        print(sys.argv[2], "Current loss: ", err, flush=True)

        with open(sys.argv[2], "w") as f:
            json.dump(data, f)

        if err < best_loss * 0.98:
            best_loss = err
        else:
            break

print(sys.argv[2], "Interp", flush=True)

lo = runs[0]["t"] - 0*(runs[1]["t"] - runs[0]["t"])
hi = runs[-1]["t"] + 0*(runs[-1]["t"] - runs[-2]["t"])
xs = np.linspace(lo, hi, nsample)

for k in ["t", "e", "cv", "b_e", "k", "chi_k", "b_k", "s", "chi_s", "b_s"]:
    data[k] = []

for index, t in enumerate(xs):
    n = runs[0]["n"]
    f0 = np.interp(t, [r["t"] for r in runs], np.concatenate(([0], data["f"])))
    f = f0 + np.log(mhr.reweighted_observable(t, f0, 1, data["f"], runs))

    e = mhr.reweighted_observable(t, f, lambda run: run["e"], data["f"], runs)
    ce2 = mhr.reweighted_observable(t, f, lambda run: (run["e"]-e)**2, data["f"], runs)
    e2 = ce2 + e**2
    e4 = mhr.reweighted_observable(t, f, lambda run: run["e"]**4, data["f"], runs)
    data["e"].append(e/n)
    data["cv"].append(ce2/t**2/n)
    data["b_e"].append(1 - e4/e2**2/3)

    if order_param == "k":
        k = mhr.reweighted_observable(t, f, lambda run: run["k"], data["f"], runs)
        ck2 = mhr.reweighted_observable(t, f, lambda run: (run["k"]-k)**2, data["f"], runs)
        k2 = ck2 + k**2
        k4 = mhr.reweighted_observable(t, f, lambda run: run["k"]**4, data["f"], runs)
        data["k"].append(k/n)
        data["chi_k"].append(ck2/t/n)
        data["b_k"].append(1 - k4/k2**2/2)
    else:
        s = mhr.reweighted_observable(t, f, lambda run: run["s"], data["f"], runs)
        cs2 = mhr.reweighted_observable(t, f, lambda run: (run["s"]-s)**2, data["f"], runs)
        s2 = cs2 + s**2
        s4 = mhr.reweighted_observable(t, f, lambda run: run["s"]**4, data["f"], runs)
        data["s"].append(s/n)
        data["chi_s"].append(cs2/t/n)
        data["b_s"].append(1 - s4/s2**2/3)

    data["t"].append(t)

    with open(sys.argv[2], "w") as of:
        json.dump(data, of) 
    print(sys.argv[2], index+1, "of", len(xs), flush=True)