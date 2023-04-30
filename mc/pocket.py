import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

height = 6
width = 6
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


def show_tiling(ax, trimer_positions, wf="hexa"):
    for x, y, s in trimer_positions:
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
    ax.set_xlim([0, 25])
    ax.set_ylim([0, 25])


def find_overlaps(trimer_positions, new_trimer):
    pass


def main():
    fig, ax = plt.subplots(1, 1, figsize=[8, 8])
    draw_hexalattice(ax)
    show_tiling(ax, [])
    plt.show()


main()
