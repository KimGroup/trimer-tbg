import matplotlib
import scipy
import matplotlib.pyplot as plt
import cProfile
import random
import matplotlib.patches as mpatches
import matplotlib.collections as mcollections
import numpy as np

height = 108
width = 108

if height != width:
    print("warning: XY or X'Y' reflections only work with square dims")

R = 1
plt.rcParams["mathtext.fontset"] = "dejavuserif"


def draw_hexalattice(ax, color=None, ls="-"):
    from matplotlib import collections as mc
    if color is None:
        color = "black"
    Hexa = []
    HexaLines = []

    for y in range(height + 1):
        xlen = width + 1
        xrange = (0, width + 1)

        for x in range(xrange[0], xrange[1]):
            Hexa.append((R * np.sqrt(3) * x + R * np.sqrt(3) / 2 * y,
                         R * np.sqrt(3) * y * np.sqrt(3) / 2 + R / 2))

            if x != xrange[0]:
                HexaLines.append((len(Hexa) - 2, len(Hexa) - 1))

            if y > 0:
                HexaLines.append((len(Hexa) - 1, len(Hexa) - xlen))
                HexaLines.append((len(Hexa) - 1, len(Hexa) - xlen - 1))

            # if x < xrange[1] - 1:
                # HexaLines.append((len(Hexa) - 1, len(Hexa) - xlen))

    ax.add_collection(mc.LineCollection(
        [(Hexa[x], Hexa[y]) for x, y in HexaLines], color=color, lw=1, ls=ls, zorder=0.5))


def trilatloc(xy, updown):
    xy = (xy[0], xy[1] + 1)

    return (
        (xy[0] + 1/2) * np.sqrt(3) * R + xy[1] * np.sqrt(3) * R / 2,
        xy[1] * np.sqrt(3) * R * np.sqrt(3) / 2
    ) if updown == "down" else (
        xy[0] * np.sqrt(3) * R + xy[1] * np.sqrt(3) * R / 2,
        xy[1] * np.sqrt(3) * R * np.sqrt(3) / 2 - R / 2
    )


def draw_duallattice(ax, color=None, ls="-"):
    if color is None:
        color = "black"

    for x in range(1, width):
        for y in range(1, height):
            xy = trilatloc((x, y), "down")
            xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
            ax.add_patch(mpatches.RegularPolygon(xy, numVertices=6, radius=R,
                         zorder=-0.5, lw=1, fill=False, edgecolor=color, ls=ls))

    xy = trilatloc((0, 0), "down")
    xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
    ax.plot([xy[0] + R*np.sqrt(3)/2, xy[0] + R*np.sqrt(3)],
            [xy[1] + R/2, xy[1] + R], color=color, lw=1, zorder=-0.5)
    xy = trilatloc((width - 1, height - 1), "down")
    xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
    ax.plot([xy[0] + R*np.sqrt(3)/2, xy[0] + R*np.sqrt(3)],
            [xy[1] + R/2, xy[1] + R], color=color, lw=1, zorder=-0.5)


def show_tiling(ax, sample, color="blue", wf="hexa", zoom=1):
    pos, occ = sample
    patches = []
    for x, y, s in pos:
        if wf == "tri":
            if s == 0:
                xy = trilatloc((x, y), "up")
                polygon = mpatches.RegularPolygon(xy, numVertices=3,
                                                  radius=R,
                                                  orientation=0)
            else:
                xy = trilatloc((x, y), "down")
                polygon = mpatches.RegularPolygon(xy, numVertices=3,
                                                  radius=R,
                                                  orientation=np.pi)
            patches.append(polygon)
        else:
            if s == 0:
                xy = trilatloc((x, y), "up")
                polygon = mpatches.Circle(xy, radius=R/3)
            else:
                xy = trilatloc((x, y), "down")
                polygon = mpatches.Circle(xy, radius=R/3)
            patches.append(polygon)
    ax.add_collection(mcollections.PatchCollection(patches, edgecolors="black", facecolors=color))

    ax.axis("off")
    ax.set_xlim([0, width*1.5*R*2*zoom])
    ax.set_ylim([0, height*np.sqrt(3)/2*R*2*zoom])
    ax.set_aspect("equal")


def find_overlaps(sample, new_trimer):
    search_list = \
        [(-1, 1, 0), (-1, 1, 1), (0, 1, 0), (0, 0, 1), (1, 0, 0), (1, -1, 0), (1, -1, 1), (0, -1, 0), (0, -1, 1), (-1, -1, 1), (-1, 0, 0), (-1, 0, 1)] if new_trimer[2] == 0 else \
        [(0, 1, 0), (0, 1, 1), (1, 1, 0), (1, 0, 0), (1, 0, 1), (1, -1, 0),
         (1, -1, 1), (0, -1, 1), (0, 0, 0), (-1, 0, 1), (-1, 1, 0), (-1, 1, 1)]

    trimers, positions = sample

    for x, y, s in search_list:
        pos = (new_trimer[0] + x, new_trimer[1] + y, s)
        if pos in positions:
            yield positions[pos]


def occupations(pos):
    if pos[2] == 0:
        return [(pos[0], pos[1]), ((pos[0] + 1) % width, pos[1]), (pos[0], (pos[1] + 1) % height)]
    else:
        return [((pos[0] + 1) % width, (pos[1] + 1) % height), ((pos[0] + 1) % width, pos[1]), (pos[0], (pos[1] + 1) % height)]


def gen_occ(l):
    d = {}
    for x in l:
        for o in occupations(x):
            d[o] = x
    return d


def rand_symmetry():
    match random.choice(["R"]):
        case "T":
            return ("T", (random.randrange(2), random.randrange(2), random.randrange(2)))
        case "R":
            return ("R", (random.randrange(width), random.randrange(height), random.randrange(6)))


def recenter_tri(pos):
    return (pos[0] % width, pos[1] % height, pos[2])

def center_tri(c, pos):
    r = recenter_mono((pos[0]-c[0]+width//2, pos[1]-c[1]+height//2))
    return r[0]-width//2, r[1]-height//2, pos[2]

def recenter_mono(pos):
    return (pos[0] % width, pos[1] % height)

def center_mono(c, pos):
    r = recenter_mono((pos[0]-c[0]+width//2, pos[1]-c[1]+height//2))
    return r[0]-width//2, r[1]-height//2

def apply_symmetry(sym, pos):
    match sym:
        case("T", (dx, dy, ds)):
            return recenter_tri((pos[0] + dx + (pos[2] + ds) // 2, pos[1] + dy, (pos[2] + ds) % 2))
        case("R", (cx, cy, dir)):
            px, py, ps = recenter_tri(
                (pos[0] - cx + width//2, pos[1] - cy + height//2, pos[2]))

            # if (width % 2 == 1 and px == width-1) or \
            #     (height % 2 == 1 and py == height-1):
            #     if 0 <= dir < 3:
            #         return (pos[0], pos[1], 1-dir)
            #     else:
            #         return (pos[0], pos[1], dir)

            px -= width//2
            py -= height//2

            match dir:
                case 0:  # X axis
                    if ps == 0:
                        return recenter_tri((cx+px+py, cy-py-1, 1))
                    else:
                        return recenter_tri((cx+px+py+1, cy-py-1, 0))
                case 1:  # Y axis
                    if ps == 0:
                        return recenter_tri((cx-px-1, cy+py+px, 1))
                    else:
                        return recenter_tri((cx-px-1, cy+py+px+1, 0))
                case 2:  # X+Y axis
                    if ps == 0:
                        return recenter_tri((cx-py-1, cy-px-1, 1))
                    else:
                        return recenter_tri((cx-py-1, cy-px-1, 0))
                case 3: # X' axis
                    if ps == 0:
                        return recenter_tri((cx-px-1-py, cy+py, 0))
                    else:
                        return recenter_tri((cx-px-2-py, cy+py, 1))
                case 4: # Y' axis
                    if ps == 0:
                        return recenter_tri((cx+px, cy-py-1-px, 0))
                    else:
                        return recenter_tri((cx+px, cy-py-2-px, 1))
                case 5: # X' + Y' axis
                    if ps == 0:
                        return recenter_tri((cx+py, cy+px, 0))
                    else:
                        return recenter_tri((cx+py, cy+px, 1))
    return pos

def mask_above(row):
    if row is None:
        return None
    places = []
    for i in range(len(row)):
        if row[i] == 1 or row[i] == -1 or row[(i+1)%len(row)] == -1:
            places.append(1)
        else:
            places.append(0)
    return places

def shape_of(mask):
    if mask is None:
        return None

    if all(x == 0 for x in mask):
        # periodic
        return len(mask), [(0, len(mask))]

    shift = mask.index(1)
    mask = mask[shift:] + mask[:shift]

    ret = []
    begin = -1
    for i in range(len(mask)):
        if mask[i] == 1:
            if begin < i - 1:
                ret.append(((begin+shift+1)%len(mask), i-begin-1))
            begin = i
    if begin < len(mask) - 1:
        ret.append(((begin+shift+1)%len(mask), len(mask)-begin-1))

    return len(mask), ret

def rotate(pos, dir):
    match dir % 6:
        case 0: return pos
        case 1: return (-pos[1], pos[0]+pos[1])
        case 2: return (-pos[0]-pos[1], pos[0])
        case 3: return (-pos[0], -pos[1])
        case 4: return (pos[1], -pos[0]-pos[1])
        case 5: return (pos[0]+pos[1], -pos[0])


def advance(shape, prev):
    size, slots = shape
    if len(slots) == 0:
        if prev is None:
            return [0] * size
        else:
            return None

    if slots[0][1] == size:
        periodic = True
        shift = 0

        if prev is not None:
            if prev[0] == 1:
                return prev[1:] + prev[:1]
            elif prev[0] == 2:
                prev = prev[-1:] + prev[:-1]
    else:
        periodic = False
        shift = slots[0][0]

    def fill_reset(begin, row):
        for start, l in slots:
            shifted_start = (start-shift)%len(row)
            if shifted_start + l >= begin:
                true_start = max(shifted_start, begin)
                true_length = shifted_start + l - true_start
                # print("fill", true_start, true_length)

                if periodic and true_length%2 == 1 and row[0] == -1:
                    # U..UDU
                    if true_length < 3:
                        return False

                    for i in range(true_start, shifted_start+l-3, 2):
                        row[i] = 1
                        row[i+1] = 2
                    row[shifted_start+l-3] = -1
                    row[shifted_start+l-2] = 1
                    row[shifted_start+l-1] = 2
                else:
                    # U..UU(D?)
                    for i in range(true_start, shifted_start+l-1, 2):
                        row[i] = 1
                        row[i+1] = 2

                    if true_length%2 == 1:
                        row[shifted_start+l-1] = -1
        return True

    if prev is None:
        ret = [0] * size
        fill_reset(0, ret)
        return ret[-shift:] + ret[:-shift]

    prev = prev[shift:] + prev[:shift]
    ret = prev.copy()

    for i in reversed(range(len(prev))):
        if i >= 3 and \
                (ret[(i+1)%len(ret)] == 0 or ret[(i+1)%len(ret)] == 1) and \
                ret[i] == 2 and ret[i-2] == 2 and \
                (i < 4 or ret[i-4] == 2 or ret[i-4] == 0):
            # match [U^]UU[U$], replace with [U^]DUD[U$]
            # print("match A", i)
            temp_ret = ret.copy()
            temp_ret[i-3] = -1
            temp_ret[i-2] = 1
            temp_ret[i-1] = 2
            temp_ret[i] = -1
            if not fill_reset(i+1, temp_ret):
                continue
            ret = temp_ret
            break
        if i >= 2 and \
                ret[i] == -1 and ret[i-1] == 2 and \
                (i < 3 or ret[i-3] == 2 or ret[i-3] == 0):
            # match [U^]UD..., replace with [U^]DU(U..?)
            # print("match B", i)
            temp_ret = ret.copy()
            temp_ret[i-2] = -1
            temp_ret[i-1] = 1
            temp_ret[i] = 2
            if not fill_reset(i+1, temp_ret):
                continue
            ret = temp_ret
            break
    else:
        return None

    return ret[-shift:] + ret[:-shift]
    

def enumerate_tilings():
    import itertools
    for init_mask in itertools.product([0, 1], repeat=width):
        print(init_mask)
        init_mask = list(init_mask)
        init_shape = shape_of(init_mask)
        init_row = advance(init_shape, None)
        dfs = [(init_shape, init_row)]
        while True:
            if dfs[-1][1] is None:
                dfs.pop()
                if len(dfs) == 0:
                    break

                next_row = advance(dfs[-1][0], dfs[-1][1])
                dfs[-1] = (dfs[-1][0], next_row)

                continue

            if len(dfs) < height:
                prev_shape = shape_of(mask_above(dfs[-1][1]))
                new_row = advance(prev_shape, None)
                dfs.append((prev_shape, new_row))
                continue

            # if dfs[1][1] == [-1, 0, 0, -1, 0, 0, -1, 1, 2, -1, 1, 2] and \
            #     dfs[2][1] == [0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0] and \
            #     dfs[3][1] == [0, 0, -1, 0, 0, 1, 2, -1, 1, 2, 1, 2] and \
            #     dfs[4][1] == [2, 0, 0, 1, 2, 0, 0, 0, 0, -1, 0, 1] and \
            #     dfs[5][1] == [1, 2, -1, 0, -1, 1, 2, -1, 0, 0, -1, 0]:
            #     print("try:")
            #     print("\n".join([" ".join(str(x) if x>=0 else "-" for x in row) for shape, row in reversed(dfs)]))
            #     if mask_above(dfs[-1][1]) == init_mask:
            #         print("passed")
            #         fig, ax = plt.subplots(1, 1, figsize=[8, 8])
            #         draw_hexalattice(ax)
            #         show_tiling(ax, (rowspace_to_trimers([row for shape, row in dfs]), None))
            #         plt.savefig("dfs.png")
            #     input()
            
            # match vertical PBC
            if mask_above(dfs[-1][1]) == init_mask:
                yield [row for shape, row in dfs]
            
            dfs[-1] = (dfs[-1][0], advance(dfs[-1][0], dfs[-1][1]))

def find_monomers(sample):
    positions, occs = sample
    locs = set()
    for x in range(width):
        for y in range(height):
            locs.add((x, y))
    for p in positions:
        for pt in occupations(p):
            locs.discard((pt))
    return locs
            
def pocket_move(sample):
    positions, occ = sample
    seed = random.choice(positions)

    pocket = ([seed], {x: seed for x in occupations(seed)})
    Abar = ([], {})
    A = (positions.copy(), occ.copy())

    A[0].remove(seed)
    for pt in occupations(seed):
        del A[1][pt]

    sym = rand_symmetry()

    while len(pocket[0]) > 0:
        el = random.choice(pocket[0])
        moved = apply_symmetry(sym, el)

        # fig, ax = plt.subplots(1, 1, figsize=[8, 8])
        # draw_hexalattice(ax)
        # show_tiling(ax, (A[0], None), "blue")
        # show_tiling(ax, (Abar[0], None), "green")
        # show_tiling(ax, (pocket[0], None), "red")
        # plt.show()

        # final location fixed: move from pocket to complement
        pocket[0].remove(el)
        for pt in occupations(el):
            del pocket[1][pt]

        Abar[0].append(moved)
        for pt in occupations(moved):
            Abar[1][pt] = moved

        for pt in occupations(moved):
            if pt in A[1]:
                overlap = A[1][pt]

                # move from A to pocket
                A[0].remove(overlap)
                pocket[0].append(overlap)
                for occ in occupations(overlap):
                    del A[1][occ]
                    pocket[1][occ] = overlap


    return (A[0] + Abar[0], A[1] | Abar[1]), len(Abar[0])


def rowspace_to_trimers(rows):
    ret = []
    for i, row in enumerate(rows):
        for j in range(len(row)):
            if row[j] == 1:
                ret.append((j, i, 0))
            if row[j] == -1:
                ret.append(((j-1)%len(row), i, 1))
    return ret

def main2():
    class Index:
        ind = 0
        def __init__(self, ax, tilings):
            self.ax = ax
            self.tilings = tilings

        def draw(self):
            print(f"{self.ind}-{self.ind + self.ax.size - 1} of {len(self.tilings)}")
            for i in range(self.ax.shape[0]):
                for j in range(self.ax.shape[1]):
                    self.ax[i, j].clear()
                    draw_hexalattice(self.ax[i, j])
                    pos = self.tilings[self.ind + i * self.ax.shape[1] + j]
                    sample = (pos, gen_occ(pos))
                    show_tiling(self.ax[i, j], sample)
            plt.draw()

        def next(self, event):
            self.ind += self.ax.size
            self.draw()

        def prev(self, event):
            self.ind -= self.ax.size
            self.draw()

    tilings_ = list(enumerate_tilings())
    tilings = [rowspace_to_trimers(x) for x in tilings_]

    fig, ax = plt.subplots(5, 3, figsize=[8, 8])
    axprev = fig.add_axes([0.7, 0.05, 0.1, 0.03])
    axnext = fig.add_axes([0.81, 0.05, 0.1, 0.03])

    from matplotlib.widgets import Button
    callback = Index(ax, tilings)
    callback.draw()

    bnext = Button(axnext, 'Next')
    bnext.on_clicked(callback.next)
    bprev = Button(axprev, 'Previous')
    bprev.on_clicked(callback.prev)

    plt.show()


def encode_trimer_list(trimers):
    return ";".join(",".join(str(i) for i in a) for a in sorted(trimers))

def main4():
    with open("12x12-tilings.dat", "w") as f:
        for x in enumerate_tilings():
            tr = rowspace_to_trimers(x)
            f.write(encode_trimer_list(tr) + "\n")

def main4_5():
    with open("12x12-tilings.dat", "r") as f:
        occs = {x.strip(): 0 for x in f.readlines()}

    print(len(occs))

    pos = []
    for i in [0, 3, 6, 9]:
        for j in [0, 3, 6, 9]:
            pos.append((i, j, 0))
            pos.append((i+1, j+1, 0))
            pos.append((i+2, j+2, 0))

    sample = (pos, gen_occ(pos))
    orig_sample = sample
    
    fitx = []
    fity = []

    # for i in range(8000000):
    #     if i > 0 and i % 5000 == 0:
    #         fitx.append(i)
    #         fity.append(len(occs))
    #         print(i, len(occs))

    #     sample, _ = pocket_move(sample)

    #     # fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    #     # draw_hexalattice(ax)
    #     # show_tiling(ax, sample)
    #     # plt.show()

    #     enc = encode_trimer_list(sample[0])
    #     if enc not in occs:
    #         fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    #         draw_hexalattice(ax)
    #         show_tiling(ax, sample)
    #         plt.savefig("asdf.png")
    #         asdf
    #         occs[enc] = 1
    #     else:
    #         occs[enc] += 1
    
    # np.save("fitx", fitx)
    # np.save("fity", fity)
    fitx = np.load("fitx.npy")
    fity = np.load("fity.npy")

    plt.plot(fitx, fity, "o")
    # def f(x, n):
    #     return n*(1-np.exp(-x/n))
    # def f(x, n):
    #     return n*(1-np.power((n-1)/n, x))

    # a, c = scipy.optimize.curve_fit(f, fitx, fity)
    # plt.plot(np.linspace(0, 5e6), f(np.linspace(0, 5e6), a[0]), label=f"best fit: {round(a[0])}+-{round(np.sqrt(c[0,0]))}")

    plt.xlabel("# MC samples")
    plt.ylabel("# unique states visited")
    plt.axhline(2758128, label="true # states from exact enum=2467326")

    plt.legend()
    plt.savefig("fig.png")

def monomer_monomer(pos):
    pos = list(pos)
    for i in range(len(pos)):
        for j in range(i+1, len(pos)):
            yield center_mono((0, 0), (pos[i][0]-pos[j][0], pos[i][1]-pos[j][1]))

def trimer_trimer(sample):
    pos, occ = sample
    for _ in range(width * height // 10):
        ij = np.random.randint(len(pos), size=2)
        i, j = ij[0], ij[1]
        if pos[j][2] == 1:
            ni = apply_symmetry(("R", (0, 0, 2)), pos[i])
            nj = apply_symmetry(("R", (0, 0, 2)), pos[j])
            yield center_tri((0, 0), (ni[0]-nj[0], ni[1]-nj[1], ni[2]))
        else:
            yield center_tri((0, 0), (pos[i][0]-pos[j][0], pos[i][1]-pos[j][1], pos[i][2]))

def monomer_dimer(pos):
    pos = list(pos)
    for i in range(len(pos)):
        for j in range(i+1, len(pos)):
            c = None
            for dx, dy, r in [(1, 0, 0), (0, 1, 1), (-1, 1, 2)]:
                if (pos[i][0] + dx) % width == pos[j][0] and (pos[i][1] + dy) % height == pos[j][1]:
                    c = pos[i]
                    rot = r
                    break
                if (pos[j][0] + dx) % width == pos[i][0] and (pos[j][1] + dy) % height == pos[i][1]:
                    c = pos[j]
                    rot = r
                    break
            
            if c is not None:
                for k in range(len(pos)):
                    if k == i or k == j:
                        continue
                    yield center_mono((0, 0), rotate(center_mono(c, pos[k]), -rot))
                    if pos[0] != (0, 0):
                        pass
                        # import pdb; pdb.set_trace()


def main5():
    global width, height
    # for x in np.arange(6, 18, 6):
    for x in [108]:
        width = x
        height = x

        pos = []
        for i in range(0, height, 3):
            for j in range(0, width, 3):
                pos.append((i, j, 0))
                pos.append((i+1, j+1, 0))
                pos.append((i+2, j+2, 0))
        pos.remove((0, 0, 0))

        sample = (pos, gen_occ(pos))

        mono_di = {}
        tri_tri = {}
        mono_mono = {}

        for i in range(10):
            sample, _ = pocket_move(sample)

        for i in range(500000):
            if i % 1000 == 0:
                print(i)

            sample, _ = pocket_move(sample)
            print(i)

            # pos = find_monomers(sample)

            if i % 5 == 0:
                # for dist in monomer_monomer(pos):
                #     if dist in mono_mono:
                #         mono_mono[dist] += 1
                #     else:
                #         mono_mono[dist] = 1

                # for dist in monomer_dimer(pos):
                #     if dist in mono_di:
                #         mono_di[dist] += 1
                #     else:
                #         mono_di[dist] = 1

                for dist in trimer_trimer(sample):
                    if dist in tri_tri:
                        tri_tri[dist] += 1
                    else:
                        tri_tri[dist] = 1

        print(tri_tri)
        # np.save(f"data/{x}x{x}-3-500000.3-mono-mono", mono_mono)
        np.save(f"data/{x}x{x}-3-500000.5-tri-tri-2", tri_tri)
        # np.save(f"data/{x}x{x}-3-100000-mono-di", mono_di)

# main()
# main2()
# main3()
# main4()
# main4_5()
# main5()