import matplotlib.pyplot as plt
import random
import matplotlib.patches as mpatches
import numpy as np

height = 6
width = 6
if height % 2 == 1 or width % 2 == 1:
    raise ValueError("reflections only work with even dims")

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
    ax.set_xlim([0, 40])
    ax.set_ylim([0, 40])


def find_overlaps(sample, new_trimer):
    search_list = \
        [(-1, 1, 0), (-1, 1, 1), (0, 1, 0), (0, 0, 1), (1, 0, 0), (1, -1, 0), (1, -1, 1), (0, -1, 0), (0, -1, 1), (-1, -1, 1), (-1, 0, 0), (-1, 0, 1)] if new_trimer[2] == 0 else \
        [(0, 1, 0), (0, 1, 1), (1, 1, 0), (1, 0, 0), (1, 0, 1), (1, -1, 0), (1, -1, 1), (0, -1, 1), (0, 0, 0), (-1, 0, 1), (-1, 1, 0), (-1, 1, 1)]

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
    match random.choice(["R"]):
        case "T":
            return ("T", (random.randrange(2), random.randrange(2)))
        case "R":
            return ("R", (random.randrange(width), random.randrange(height), random.randrange(3)))


def recenter(pos):
    return (pos[0] % width, pos[1] % height, pos[2])


def apply_symmetry(sym, pos):
    match sym:
        case ("T", (dx, dy)):
            return recenter((pos[0] + dx, pos[1] + dy, pos[2]))
        case ("R", (cx, cy, dir)):
            px, py, ps = recenter((pos[0] - cx + width//2, pos[1] - cy + height//2, pos[2]))
            px -= width//2
            py -= height//2
            match dir:
                case 0: # X axis
                    if ps == 0:
                        return recenter((cx+px+py, cy-py-1, 1))
                    else:
                        return recenter((cx+px+py+1, cy-py-1, 0))
                case 1: # Y axis
                    if ps == 0:
                        return recenter((cx-px-1, cy+py+px, 1))
                    else:
                        return recenter((cx-px-1, cy+py+px+1, 0))
                case 2: # X+Y axis
                    if ps == 0:
                        return recenter((cx-py-1, cy-px-1, 1))
                    else:
                        return recenter((cx-py-1, cy-px-1, 0))
    return pos


def generate_random_tiling():
    pass
    
def pocket_move(sample):
    def move(pair, old, new):
        pair[0][pair[0].index(el)] = new

        for occ in occupations(old):
            del pair[1][occ]
        for occ in occupations(new):
            pair[1][occ] = new

    positions, occ = sample
    seed = random.choice(positions)

    pocket = ([seed], {x: seed for x in occupations(seed)})
    Abar = (pocket[0].copy(), pocket[1].copy())
    A = (positions.copy(), occ.copy())

    A[0].remove(seed)
    for occ in occupations(seed):
        del A[1][occ]

    sym = rand_symmetry()

    while len(pocket[0]) > 0:
        el = random.choice(pocket[0])
        moved = apply_symmetry(sym, el)

        pocket[0].remove(el)
        for occ in occupations(el):
            del pocket[1][occ]

        move(Abar, el, moved)

        for occ in occupations(moved):
            if occ in A[1]:
                overlap = A[1][occ]

                A[0].remove(overlap)
                Abar[0].append(overlap)
                pocket[0].append(overlap)
                for occ in occupations(overlap):
                    del A[1][occ]
                    Abar[1][occ] = overlap
                    pocket[1][occ] = overlap


    return (A[0] + Abar[0], A[1] | Abar[1]), len(Abar[0])


def main():
    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    pos = [(0, 0, 0), (3, 0, 0), (1, 1, 0), (4, 1, 0), (2, 2, 0), (5, 2, 0), (0, 3, 0), (3, 3, 0), (1, 4, 0), (4, 4, 0), (2, 5, 0), (5, 5, 0)]
    sample = (pos, gen_occ(pos))
    show_tiling(ax, sample)

    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    updated, N = pocket_move(sample)
    show_tiling(ax, updated)
    print(N)
    plt.show()


main()
