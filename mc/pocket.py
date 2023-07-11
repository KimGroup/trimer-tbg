import matplotlib.pyplot as plt
import random
import matplotlib.patches as mpatches
import numpy as np

random.seed(0)

height = 6
width = 6
if height % 2 == 1 or width % 2 == 1:
    print("warning: reflections only work with even dims")

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


def show_tiling(ax, sample, wf="hexa"):
    pos, occ = sample
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
            ax.add_patch(polygon)
        else:
            if s == 0:
                xy = trilatloc((x, y), "up")
                ax.add_patch(mpatches.Circle(xy, radius=R/3,
                                             facecolor="blue", ec='k'))
            else:
                xy = trilatloc((x, y), "down")
                ax.add_patch(mpatches.Circle(xy, radius=R/3,
                                             facecolor="blue", ec='k'))

    ax.axis("off")
    ax.set_xlim([0, 24])
    ax.set_ylim([0, 16])
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
        return [(pos[0], pos[1]), ((pos[0] + 1) % width, pos[1]), (pos[0], (pos[1] + 1) % width)]
    else:
        return [((pos[0] + 1) % width, (pos[1] + 1) % height), ((pos[0] + 1) % width, pos[1]), (pos[0], (pos[1] + 1) % height)]


def gen_occ(l):
    d = {}
    for x in l:
        for o in occupations(x):
            d[o] = x
    return d


def rand_symmetry():
    match random.choice(["T", "R"]):
        case "T":
            return ("T", (random.randrange(2), random.randrange(2), random.randrange(2)))
        case "R":
            return ("R", (random.randrange(width), random.randrange(height), random.randrange(3)))


def recenter(pos):
    return (pos[0] % width, pos[1] % height, pos[2])


def apply_symmetry(sym, pos):
    match sym:
        case("T", (dx, dy, ds)):
            return recenter((pos[0] + dx + (pos[2] + ds) // 2, pos[1] + dy, (pos[2] + ds) % 2))
        case("R", (cx, cy, dir)):
            
            px, py, ps = recenter(
                (pos[0] - cx + width//2, pos[1] - cy + height//2, pos[2]))
            px -= width//2
            py -= height//2
            match dir:
                case 0:  # X axis
                    if ps == 0:
                        return recenter((cx+px+py, cy-py-1, 1))
                    else:
                        return recenter((cx+px+py+1, cy-py-1, 0))
                case 1:  # Y axis
                    if ps == 0:
                        return recenter((cx-px-1, cy+py+px, 1))
                    else:
                        return recenter((cx-px-1, cy+py+px+1, 0))
                case 2:  # X+Y axis
                    if ps == 0:
                        return recenter((cx-py-1, cy-px-1, 1))
                    else:
                        return recenter((cx-py-1, cy-px-1, 0))
    return pos

def enumerate_tilings():
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
                ret[i-3] = -1
                ret[i-2] = 1
                ret[i-1] = 2
                ret[i] = -1
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
    
    # shape = (12, [(0, 12)])
    # x = advance(shape, None)
    # while x is not None:
    #     print(x)
    #     x = advance(shape, x)

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
            
            # match vertical PBC
            if mask_above(dfs[-1][1]) == init_mask:
                yield [row for shape, row in dfs]
            
            dfs[-1] = (dfs[-1][0], advance(dfs[-1][0], dfs[-1][1]))

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
    print(sym)

    while len(pocket[0]) > 0:
        el = random.choice(pocket[0])
        moved = apply_symmetry(sym, el)

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


def main():
    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    pos = [(0, 0, 0), (3, 0, 0), (1, 1, 0), (4, 1, 0), (2, 2, 0), (5, 2, 0),
           (0, 3, 0), (3, 3, 0), (1, 4, 0), (4, 4, 0), (2, 5, 0), (5, 5, 0)]
    sample = (pos, gen_occ(pos))
    show_tiling(ax, sample)

    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    updated, N = pocket_move(sample)
    show_tiling(ax, updated)
    print(N)
    plt.show()

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

def main3():
    tilings = [rowspace_to_trimers(x) for x in enumerate_tilings()]
    occs = {frozenset(x): 0 for x in tilings}

    pos = [(0, 0, 0), (3, 0, 0), (1, 1, 0), (4, 1, 0), (2, 2, 0), (5, 2, 0),
           (0, 3, 0), (3, 3, 0), (1, 4, 0), (4, 4, 0), (2, 5, 0), (5, 5, 0)]
    sample = (pos, gen_occ(pos))
    orig_sample = sample

    for i in range(5000):
        sample, _ = pocket_move(sample)

        # fig, ax = plt.subplots(1, 1, figsize=[8, 8])
        # draw_hexalattice(ax)
        # show_tiling(ax, sample)
        # plt.show()

        fig, ax = plt.subplots(1, 1, figsize=[8, 8])
        draw_hexalattice(ax)
        show_tiling(ax, sample)
        plt.show()

        occs[frozenset(sample[0])] += 1

    print(occs.values())
    print(len(occs), sum(x > 0 for x in occs.values()))

    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    sample = (tilings[9], gen_occ(tilings[9]))
    print(occs[frozenset(tilings[9])])
    show_tiling(ax, sample)

    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    show_tiling(ax, orig_sample)

    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    sample = (tilings[8], gen_occ(tilings[8]))
    print(occs[frozenset(tilings[8])])
    show_tiling(ax, sample)

    plt.show()


# main()
# main2()
main3()
