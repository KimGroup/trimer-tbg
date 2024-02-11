import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib

def read_accumulator(fname, skip=0, take=None):
    state = "count"
    total = 0

    n = 0
    mean = None
    m2 = None

    skipped = 0

    with open(fname) as f:
        for index, line in enumerate(f):
            if take is not None and n > take: break

            if state == "count":
                state = "val"
                n = int(line.strip())
            elif state == "val":
                state = "m2"
                if skipped >= skip:
                    curval = np.array([float(x) for x in line.strip().split()])
            elif state == "m2":
                state = "count"
                if skipped < skip:
                    skipped += n
                else:
                    curm2 = np.array([float(x) for x in line.strip().split()])
                    if mean is None:
                        mean = curval
                        m2 = curm2
                        total = n
                    else:
                        delta = curval - mean
                        mean = (total * mean + curval * n) / (total + n)
                        m2 = m2 + curm2 + np.power(delta, 2) * n * total / (n + total)
                        total += n

    if m2 is None:
        raise ValueError(fname)

    return mean, np.sqrt(m2/total), total


def read_accumulator_raw(fname):
    n, val, std = [], [], []
    state = "count"
    with open(fname) as f:
        for index, line in enumerate(f):
            if state == "count":
                n.append(int(line.strip()))
                state = "val"
            elif state == "val":
                val.append(np.array([float(x) for x in line.strip().split()]))
                state = "m2"
            elif state == "m2":
                std.append(np.sqrt(np.array([float(x) for x in line.strip().split()]) / n[-1]))
                state = "count"
    return np.array(val), np.array(std), np.array(n)


def enum_files(dir):
    import glob
    for fname in glob.glob(dir):
        basename = fname.split("/")[-2]
        tokens = basename.split("_")
        yield { "fname": fname, "l": int(tokens[0].split("x")[0]), "r": int(tokens[1][1:]), "t": float(tokens[2][1:]), "j4": float(tokens[3][1:]) }

def get_all_data(glob, transform, filter=lambda _: True, skip=0, by="t", with_counts=False, take=None):
    ts0, ds0, ns = [], [], []
    for props in sorted(enum_files(glob), key=lambda x: x[by]):
        if not filter(props): continue
        ts0.append(props if by == "fname" else props[by])
        data = read_accumulator(props["fname"], skip=skip, take=take)
        ds0.append(transform(data, props))
        ns.append(data[2])

    ts0 = np.array(ts0)
    
    try:
        ds0 = np.array(ds0)
    except ValueError:
        ds0 = np.array(ds0, dtype=object)

    if with_counts:
        return ts0, ds0, np.array(ns)
    else:
        return ts0, ds0

def get_all_energies(glob, skip=0, take=None, by="t"):
    return get_all_data(glob, lambda data, props: data[0][2]/props["l"]**2*3, skip=skip, take=take, by=by)

def get_all_cvs(glob, skip=0, take=None, by="t"):
    return get_all_data(glob, lambda data, props:
                        np.zeros_like(data[1][2]) if props["t"] == 0 else
                        data[1][2]**2/props["t"]**2/(props["l"]**2/3 - props["r"]/3), skip=skip, take=take, by=by)

def autocorr(vals, label=None):
    vals = np.array(vals)
    bvar = np.var(vals, ddof=1)
    origlen = len(vals)
    xs = [1]
    ys = [bvar]

    while len(vals) > 65:
        N = 2*(len(vals)//2)
        vals = (vals[:N:2] + vals[1:N:2])/2

        xs.append(xs[-1] * 2)
        ys.append(np.var(vals, ddof=1))

    xs = np.array(xs)
    ys = np.array(ys)

    plt.plot(xs, ys * xs / bvar, label=label)
    plt.xlabel("bin size")
    plt.ylabel("$\\tau$")

def bin(data, width):
    if isinstance(data, list):
        data = np.array(data)
    return data[:(data.size // width) * width].reshape(-1, width).mean(axis=1)

def plot2d_old(ax, d, l, scale=None, title=None):
    data = np.zeros((l, l))
    for key, value in d.items():
        data[key[0]+l//2, key[1]+l//2] = value

    data[l//2, l//2]=0
    if scale is not None:
        data = scale(data)

    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []
    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))
    for i in range(-l//2, l//2):
        for j in range(-l//2, l//2):
            patches.append(matplotlib.patches.Circle((i+j/2, j*np.sqrt(3)/2), radius=0.5))
            colors.append(matplotlib.colormaps["viridis"](N(data[(i+l//2), (j+l//2)])))
    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(f"monomer-monomer correlation L={l} filling=-1" if title is None else title)

def plot2d(ax, data, log=False, show_dimer=False):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []

    if log:
        data = data + 0.000001
        N = matplotlib.colors.LogNorm(vmin=np.amin(data), vmax=np.amax(data))
    else:
        N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))

    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            colors.append(matplotlib.colormaps["viridis"](N(data[i, j])))
            if i >= data.shape[0]//2:
                i -= data.shape[0]
            if j >= data.shape[1]//2:
                j -= data.shape[1]
            patches.append(matplotlib.patches.RegularPolygon((i+j/2, j*np.sqrt(3)/2), numVertices=6, radius=1/np.sqrt(3), orientation=0))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    if show_dimer:
        ax.add_patch(mpatches.Rectangle((-0.25, -0.25), 1.5, 0.5, ec=None, fc="white", zorder=1))

    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)

def plot2d_hex(ax, data, title=None):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []
    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))

    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            for k in range(2):
                colors.append(matplotlib.colormaps["viridis"](N(data[i, j, k])))
                if i >= data.shape[0]//2:
                    i -= data.shape[0]
                if j >= data.shape[1]//2:
                    j -= data.shape[1]
                if k == 1:
                    dx, dy = 1, 1 / np.sqrt(3)
                else:
                    dx, dy = 0.5, 0.5 / np.sqrt(3)

                patches.append(matplotlib.patches.RegularPolygon((i+j/2+dx, j*np.sqrt(3)/2+dy), numVertices=3, radius=1/np.sqrt(3), orientation=k*np.pi))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def plot2d_arb(ax, data, xs, ys, title=None):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []

    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))
    for d, x, y in zip(data, xs, ys):
        patches.append(matplotlib.patches.RegularPolygon((x, y), numVertices=6, radius=0.5/np.sqrt(3)))
        colors.append(matplotlib.colormaps["viridis"](N(d)))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def FT_hex(vals):
    newvals = []
    newx = []
    newy = []

    radius = vals.shape[0]/4.5

    def add(x, y, s):
        if s == 1:
            xy = (x + y * 0.5 + 0.5, y * np.sqrt(3) / 2 + 1/np.sqrt(3)/2)
        else:
            xy = (x + y * 0.5, y * np.sqrt(3) / 2)

        if (xy[0]**2 + xy[1]**2 < (radius*3)**2):
            newx.append(xy[0])
            newy.append(xy[1])
            newvals.append(vals[x%vals.shape[0], y%vals.shape[1], s] * np.exp(-(xy[0]**2+xy[1]**2)/(2*radius**2)))

    bounding = int(vals.shape[0]*2)
    for i in range(bounding):
        for j in range(bounding):
            add(i-bounding//2, j-bounding//2, 0)
            add(i-bounding//2, j-bounding//2, 1)

    newx = np.array(newx)
    newy = np.array(newy)
    newvals = np.array(newvals)

    def corr(k):
        return np.sum(np.exp(-1j * (newx * k[0] * np.pi + newy * k[1] * np.pi)) * newvals)
    return corr

def FT_hex2(vals):
    newvals = []
    newx = []
    newy = []

    nrows = (vals.shape[0] // 2) * 2 + 1
    halfnrows = nrows // 2

    def add(x, y, s):
        newvals.append(vals[x%vals.shape[0], y%vals.shape[1], s])
        if s == 1:
            newx.append(x + y * 0.5 + 0.5)
            newy.append(y * np.sqrt(3)/2 + 1/np.sqrt(3)/2)
        else:
            newx.append(x + y * 0.5)
            newy.append(y * np.sqrt(3)/2)

    for row in range(-halfnrows, halfnrows+1):
        if row < 0:
            rowsize = nrows + row
            rowstart = (-halfnrows-row, row)

            add(rowstart[0]-1, rowstart[1], 1)
            for col in range(rowsize):
                add(rowstart[0]+col, rowstart[1], 0)
                add(rowstart[0]+col, rowstart[1], 1)
        else:
            rowsize = nrows - row - 1
            rowstart = (-halfnrows, row)

            for col in range(rowsize):
                add(rowstart[0]+col, rowstart[1], 0)
                add(rowstart[0]+col, rowstart[1], 1)
            add(rowstart[0]+rowsize, rowstart[1], 0)

    newx = np.array(newx)
    newy = np.array(newy)
    newvals = np.array(newvals)

    def corr(k):
        return np.sum(np.exp(-1j * (newx * k[0] * np.pi + newy * k[1] * np.pi)) * newvals)

    return corr


def FT_hex3(vals):
    newvals = []
    newx = []
    newy = []

    maxwidth = (vals.shape[0] // 3) * 3
    nmaxrows = (maxwidth // 3) * 2 + 1
    ncornerrows = (maxwidth // 3)

    def add(x, y, s):
        newvals.append(vals[x%vals.shape[0], y%vals.shape[1], s])
        if s == 1:
            newx.append(x + y * 0.5 + 0.5)
            newy.append(y * np.sqrt(3) / 2 + 1/np.sqrt(3)/2)
        else:
            newx.append(x + y * 0.5)
            newy.append(y * np.sqrt(3) / 2)

    # bulk
    for row in range(nmaxrows):
        rowstart = (-(nmaxrows//2) - row//2, row - (nmaxrows//2))
        for col in range(maxwidth):
            add(rowstart[0]+col, rowstart[1], 0)
            add(rowstart[0]+col, rowstart[1], 1)
        if row % 2 == 0:
            add(rowstart[0]+maxwidth, rowstart[1], 0)
        else:
            add(rowstart[0]-1, rowstart[1], 1)

    # lower corner
    for row in range(ncornerrows):
        rowstart = (-ncornerrows+(row+1)*2, -ncornerrows-(row+1))
        width = maxwidth-2-row*3
        add(rowstart[0]-1, rowstart[1], 1)
        for col in range(width):
            add(rowstart[0]+col, rowstart[1], 0)
            add(rowstart[0]+col, rowstart[1], 1)

    # # upper corner
    for row in range(ncornerrows):
        rowstart = (-ncornerrows*2+row+1, ncornerrows+(row+1))
        width = maxwidth-3-row*3
        for col in range(width):
            add(rowstart[0]+col, rowstart[1], 0)
            add(rowstart[0]+col, rowstart[1], 1)
        add(rowstart[0]+width, rowstart[1], 0)

    newx = np.array(newx)
    newy = np.array(newy)
    newvals = np.array(newvals)

    def corr(k):
        return np.sum(np.exp(-1j * (newx * k[0] * np.pi + newy * k[1] * np.pi)) * newvals)

    return corr


coords = None
extent = 2.5
def make_coords():
    global coords
    if coords is not None:
        return

    X = np.linspace(-extent, extent, 400)
    Y = np.linspace(-extent, extent, 400)

    coords = np.zeros((len(X), len(Y), 2))
    for x in range(coords.shape[0]):
        for y in range(coords.shape[1]):
            xy = np.array([X[x], Y[y]])

            # fold into fundamental domain
            origins = list(map(np.array, [(0, 0), (0, 0), (2, 0)]))
            normals = list(map(np.array, [(0, 1), (np.sin(np.pi/6), -np.cos(np.pi/6)), (-1, 0)]))
            while True:
                for o, n in zip(origins, normals):
                    sxy = xy - o
                    if np.dot(sxy, n) < 0:
                        sxy = sxy - (2 * np.dot(sxy, n) * n)
                        xy = sxy + o
                        break
                else:
                    break

            coords[x, y] = [xy[0], xy[1]]


def plot_FT(ax, ft, proj="re", fold=True):
    make_coords()

    if fold:
        datares = 75
        dataX = np.linspace(-0.02, 2.02, datares * 2)
        dataY = np.linspace(-0.02, 2.02/np.sqrt(3), datares)
        data = np.zeros((datares * 2, datares), dtype=complex)
        for x in range(datares * 2):
            for y in range(min(datares, x//2 + 3)):
                data[x, y] = ft((dataX[x], dataY[y]))
        import scipy
        interp = scipy.interpolate.RegularGridInterpolator((dataX, dataY), data)
        interped = interp(coords)
    else:
        datares = 50
        dataX = np.linspace(-extent, extent, datares*2)
        dataY = np.linspace(-extent, extent, datares*2)
        data = np.zeros((datares*2, datares*2), dtype=complex)
        for x in range(datares*2):
            for y in range(datares*2):
                data[x, y] = ft((dataX[x], dataY[y]))
        interped = data

    if proj == "re":
        mapped = np.real(interped)
        abs = np.amax(np.abs(mapped))
        norm = matplotlib.colors.Normalize(vmin=-abs, vmax=abs)
        cmap = "RdBu"
    elif proj == "im":
        mapped = np.imag(interped)
        abs = np.amax(np.abs(mapped))
        norm = matplotlib.colors.Normalize(vmin=-abs, vmax=abs)
        cmap = "RdBu"
    elif proj == "abs":
        mapped = np.abs(interped)
        norm = matplotlib.colors.Normalize(vmin=np.amin(mapped), vmax=np.amax(mapped))
        cmap = "viridis"
    elif proj == "logabs":
        mapped = np.abs(interped) + 0.0001
        norm = matplotlib.colors.LogNorm(vmin=np.amin(mapped), vmax=np.amax(mapped))
        cmap = "viridis"
    elif proj == "logre":
        mapped = np.abs(np.real(interped)) + 0.0001
        norm = matplotlib.colors.LogNorm(vmin=np.amin(mapped), vmax=np.amax(mapped))
        cmap = "viridis"


    ax.imshow(mapped.T, origin="lower", extent=(-extent, extent, -extent, extent), cmap=cmap, norm=norm)
    ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/np.sqrt(3), fill=None, ec="k", alpha=0.2, lw=0.5))
    ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/3, fill=None, ec="k", orientation=np.pi/6, alpha=0.2, lw=0.5))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm, cmap=cmap), ax=ax)


def plot_timeseries(ax, data, std, label=None):
    xs = np.arange(len(data))
    ax.plot(xs, data, label=label)
    ax.fill_between(xs, data-std, data+std, alpha=0.4)