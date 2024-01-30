import numpy as np
import matplotlib.pyplot as plt

def read_accumulator(fname, skip=0, take=None):
    state = "count"
    total = 0

    n = 0
    mean = None
    m2 = None

    with open(fname) as f:
        for index, line in enumerate(f):
            if index < skip*3: continue
            if take is not None and index > (skip + take)*3: break

            if state == "count":
                n = int(line.strip())
                state = "val"
            elif state == "val":
                curval = np.array([float(x) for x in line.strip().split()])
                state = "m2"
            elif state == "m2":
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
                state = "count"

    if m2 is None:
        raise ValueError(fname)

    return mean, np.sqrt(m2/total)


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
                std.append(np.array([float(x) for x in line.strip().split()]) / n[-1])
                state = "count"
    return np.array(n), np.array(val), np.array(std)



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

def plot2d_old(d, l, scale=None, title=None):
    data = np.zeros((l, l))
    for key, value in d.items():
        data[key[0]+l//2, key[1]+l//2] = value

    data[l//2, l//2]=0
    if scale is not None:
        data = scale(data)

    fig, ax = plt.subplots(1, 1, figsize=[6, 4])
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    import matplotlib
    patches = []
    colors = []
    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))
    for i in range(-l//2, l//2):
        for j in range(-l//2, l//2):
            patches.append(matplotlib.patches.Circle((i+j/2, j*np.sqrt(3)/2), radius=0.5))
            colors.append(matplotlib.colormaps["viridis"](N(data[(i+l//2), (j+l//2)])))
    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    fig.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(f"monomer-monomer correlation L={l} filling=-1" if title is None else title)

def plot2d(ax, data, title=None):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    import matplotlib
    patches = []
    colors = []
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
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def plot2d_hex(ax, data, title=None):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    import matplotlib
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

    import matplotlib
    patches = []
    colors = []

    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))
    for d, x, y in zip(data, xs, ys):
        patches.append(matplotlib.patches.RegularPolygon((x, y), numVertices=6, radius=1/np.sqrt(3)))
        colors.append(matplotlib.colormaps["viridis"](N(d)))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def FT_hex(vals, shift=(0,0)):
    positions_x = np.zeros_like(vals)
    positions_y = np.zeros_like(vals)

    for x in range(positions_x.shape[0]):
        for y in range(positions_x.shape[1]):
            for s in range(positions_x.shape[2]):
                if x >= vals.shape[0]//2:
                    x -= vals.shape[0]
                if y >= vals.shape[1]//2:
                    y -= vals.shape[1]

                positions_x[x, y, s] = x + y * 0.5
                positions_y[x, y, s] = y * np.sqrt(3) / 2
                if s == 1:
                    positions_x[x, y, s] += 0.5
                    positions_y[x, y, s] += 1/np.sqrt(3)/2

    # positions_x -= (np.amax(positions_x)-np.amin(positions_x))/2
    # positions_y -= (np.amax(positions_y)-np.amin(positions_y))/2

    positions_x += shift[0]
    positions_y += shift[1]

    def corr(k):
        return np.sum(np.exp(-1j * (positions_x * k[0] * np.pi + positions_y * k[1] * np.pi)) * vals)

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
            newy.append(y * np.sqrt(3) / 2 + 1/np.sqrt(3)/2)
        else:
            newx.append(x + y * 0.5)
            newy.append(y * np.sqrt(3) / 2)

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



def plot_FT(ax, ft, proj="re"):
    extent = 2.35
    X, Y = np.meshgrid(np.linspace(-extent, extent, 200), np.linspace(-extent, extent, 200))
    cslft = np.zeros_like(X, dtype=complex)
    for kx in range(cslft.shape[0]):
        for ky in range(cslft.shape[1]):
            cslft[kx, ky] = ft((X[kx, ky], Y[kx, ky]))

    import matplotlib.patches as mpatches
    import matplotlib
    # data = np.log(np.abs(cslft)+1)

    if proj == "re":
        data = np.real(cslft)
        abs = np.amax(np.abs(data))
        norm = matplotlib.colors.Normalize(vmin=-abs, vmax=abs)
        cmap = "RdBu"
    elif proj == "im":
        data = np.imag(cslft)
        abs = np.amax(np.abs(data))
        norm = matplotlib.colors.Normalize(vmin=-abs, vmax=abs)
        cmap = "RdBu"
    elif proj == "abs":
        data = np.abs(cslft)
        norm = matplotlib.colors.Normalize(vmin=np.amin(data), vmax=np.amax(data))
        cmap = "viridis"
    elif proj == "logabs":
        data = np.abs(cslft) + 1
        norm = matplotlib.colors.LogNorm(vmin=np.amin(data), vmax=np.amax(data))
        cmap = "viridis"

    ax.imshow(data, origin="lower", extent=(-extent, extent, -extent, extent), cmap=cmap, norm=norm)
    ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/np.sqrt(3), fill=None, ec="k", alpha=0.5, lw=0.5))
    ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/3, fill=None, ec="k", orientation=np.pi/6, alpha=0.5, lw=0.5))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm, cmap=cmap), ax=ax)