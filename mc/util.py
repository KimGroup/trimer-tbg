import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import matplotlib

def topo_sectors(pos, lines, w, h):
    mpos = set(pos)
    vals = []
    for start, dir in lines:
        cpos = start
        total = 0
        while True:
            if cpos in mpos:
                total += 1

            if dir == 0:
                if cpos[2] == 0:
                    cpos = (cpos[0], cpos[1], 1)
                else:
                    cpos = (cpos[0] + 1, cpos[1] + 1, 0)
            elif dir == 1:
                if cpos[2] == 0:
                    cpos = (cpos[0] - 1, cpos[1] + 1, 1)
                else:
                    cpos = (cpos[0], cpos[1] + 1, 0)
            elif dir == 2:
                if cpos[2] == 0:
                    cpos = (cpos[0] - 1, cpos[1], 1)
                else:
                    cpos = (cpos[0] - 1, cpos[1] + 1, 0)

            cpos = (cpos[0] % w, cpos[1] % h, cpos[2])

            if cpos == start:
                break
        vals.append(total - (w+h)//6)
    return vals

def winding_numbers(pos, w, h):
    secs = topo_sectors(pos, [((0, 0, 0), 0), ((1, 0, 0), 1), ((1, 0, 0), 0), ((2, 0, 0), 1)], w, h)

    ri, rj = (2*secs[0]+secs[1])//3, (secs[0]-secs[1])//3
    gi, gj = (2*secs[2]+secs[3])//3, (secs[2]-secs[3])//3
    return ri, rj, gi, gj

def conf_to_string(conf):
    return " ".join("(" + ",".join(str(y) for y in x) + ")" for x in conf)

def read_histogram(fname, skip=0):
    with open(fname, mode="rb") as f:
        f.seek(0, 2)
        fsize = f.tell()
        f.seek(skip*12, 0)
        buf = f.read(fsize - skip*12)
        entries = len(buf) // 12

        e = np.ndarray((entries,), dtype=np.single, buffer=buf, strides=(12,)).astype(np.double)
        k = np.ndarray((entries,), dtype=np.single, buffer=buf, strides=(12,), offset=4).astype(np.double)
        s = np.ndarray((entries,), dtype=np.int32, buffer=buf, strides=(12,), offset=8).astype(np.double)
        return e, k, s

def read_histogram_sector(fname, skip=0):
    with open(fname, mode="rb") as f:
        f.seek(0, 2)
        fsize = f.tell()
        f.seek(skip*12, 0)
        buf = f.read(fsize - skip*12)
        entries = len(buf) // 12

        e = np.ndarray((entries,), dtype=np.single, buffer=buf, strides=(12,)).astype(np.double)
        w = np.ndarray((entries,4), dtype=np.short, buffer=buf, strides=(12,2), offset=4).astype(np.double)
        return e, w

def read_histogram_winding_dimer(fname, skip=0):
    with open(fname, mode="rb") as f:
        f.seek(0, 2)
        fsize = f.tell()
        f.seek(skip*4, 0)
        buf = f.read(fsize - skip*4)
        entries = len(buf) // 4

        w = np.ndarray((entries,2), dtype=np.short, buffer=buf, strides=(4,2), offset=0).astype(np.double)
        return w

def calculate_moments(data):
    m1 = np.mean(data)
    m2 = np.mean(data**2)
    # m3 = np.mean(data**3)
    m4 = np.mean(data**4)

    cm2 = m2 - m1**2
    # cm4 = m4 - 4*m1*m3 + 6*m1**2*m2 - 3*m1**4
    return (1, m1, m2, None, m4), (1, 0, cm2)

def gen_colors(curves, cmap="jet"):
    if not isinstance(curves[0], tuple):
        curves = sorted(list(set(curves)))
        return {curves[i]: matplotlib.colormaps[cmap](0.8*i/(len(curves)-1) if len(curves)>1 else 1) for i in range(len(curves))}
    else:
        return {curves[i][0]: matplotlib.colormaps[cmap](0.8*i/(len(curves)-1) if len(curves)>1 else 1) for i in range(len(curves))}

def parse_positions(l):
    pos = []
    for entry in l.strip().split(" "):
        if entry == "": continue
        tokens = entry[1:-1].split(",")
        pos.append((int(tokens[0]), int(tokens[1]), int(tokens[2])))
    return pos

def read_positions(f, stride=1, skip=0, take=None):
    pos = []
    with open(f, "r") as f:
        for index, l in enumerate(f):
            if index < skip:
                continue
            if take is not None and index > skip + take:
                break

            if index % stride == 0:
                pos.append(parse_positions(l))
    return pos

def trimer_coords(x, y, s):
    if s == 1:
        return (x + y/2 + 1, y * np.sqrt(3)/2 + 1/np.sqrt(3))
    else:
        return (x + y/2 + 1/2, y * np.sqrt(3)/2 + 1/np.sqrt(3)/2)

def mono_coords(x, y, _=0):
    return (x + y/2, y * np.sqrt(3)/2)

def dimer_hex_coords(x, y, s):
    xy = mono_coords(x, y)
    angle = np.pi/6 + 2*np.pi/3*s
    d = np.sqrt(3)/6

    xy = (xy[0]+d*np.cos(angle), xy[1]+d*np.sin(angle))
    s, c = np.sin(np.pi/6), np.cos(np.pi/6)

    xy = (np.sqrt(3)*(xy[0]*c + xy[1]*s), np.sqrt(3)*(xy[1]*c - xy[0]*s))
    return xy

def draw_hexalattice(ax, width, height, color=None, ls="-"):
    if color is None:
        color = "black"
    R = 1
    vertices = []
    lines = []

    for y in range(height + 1):
        xlen = width + 1
        xrange = (0, width + 1)

        for x in range(xrange[0], xrange[1]):
            vertices.append(mono_coords(x, y))

            if x != xrange[0]:
                lines.append((len(vertices) - 2, len(vertices) - 1))

            if y > 0:
                lines.append((len(vertices) - 1, len(vertices) - xlen))
                lines.append((len(vertices) - 1, len(vertices) - xlen - 1))

            # if x < xrange[1] - 1:
            #     lines.append((len(vertices) - 1, len(vertices) - xlen))

    ax.add_collection(matplotlib.collections.LineCollection(
        [(vertices[x], vertices[y]) for x, y in lines], color=color, lw=1, ls=ls, zorder=0.5))


def draw_duallattice(ax, width, height, color=None, ls="-", zorder=-0.5):
    if color is None:
        color = "black"

    R = 1 / np.sqrt(3)

    for x in range(-1, width):
        for y in range(-1, height):
            xy = trimer_coords(x, y, 1)
            xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
            ax.add_patch(mpatches.RegularPolygon(xy, numVertices=6, radius=R,
                         zorder=zorder, lw=1, fill=False, edgecolor=color, ls=ls))

    xy = trimer_coords(0, 0, 1)
    xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
    ax.plot([xy[0] + R*np.sqrt(3)/2, xy[0] + R*np.sqrt(3)],
            [xy[1] + R/2, xy[1] + R], color=color, lw=1, zorder=zorder)
    xy = trimer_coords(width - 1, height - 1, 1)
    xy = (xy[0] - R * np.sqrt(3) / 2, xy[1] + R / 2)
    ax.plot([xy[0] + R*np.sqrt(3)/2, xy[0] + R*np.sqrt(3)],
            [xy[1] + R/2, xy[1] + R], color=color, lw=1, zorder=zorder)


def enum_starofdavid(mono_pos, w, h):
    x, y = mono_pos
    l = [
        (x, y, 1), (x-1, y+1, 0), (x-2, y, 1),
        (x-1, y-1, 0), (x, y-2, 1), (x+1, y-1, 0)
    ]

    return [(x%w, y%h, s) for (x, y, s) in l]

def enum_plaquette(mono_pos, w, h):
    x, y = mono_pos
    l = [
        (x, y, 0), (x-1, y, 1), (x-1, y, 0),
        (x-1, y-1, 1), (x, y-1, 0), (x, y-1, 1)
    ]

    return [(x%w, y%h, s) for (x, y, s) in l]

def flip_monomer(mono_pos, dir, w, h):
    x, y = mono_pos
    if dir == 0:
        x, y = x+1, y+1
    elif dir == 1:
        x, y = x-1, y+2
    elif dir == 2:
        x, y = x-2, y+1
    elif dir == 3:
        x, y = x-1, y-1
    elif dir == 4:
        x, y = x+1, y-2
    elif dir == 5:
        x, y = x+2, y-1

    return x%w, y%h

def flip_trimer(mono_pos, dir, w, h):
    x, y = mono_pos
    if dir == 0:
        x, y, s = x, y, 1
    elif dir == 1:
        x, y, s = x-1, y+1, 0
    elif dir == 2:
        x, y, s = x-2, y, 1
    elif dir == 3:
        x, y, s = x-1, y-1, 0
    elif dir == 4:
        x, y, s = x, y-2, 1
    elif dir == 5:
        x, y, s = x+1, y-1, 0

    return x%w, y%h, s

def flip_line(positions, start, w, h, lim=99999):
    newpos = set(positions)
    cpos = start
    prev_trimer = None
    flipped = set()
    for j in range(lim):
        index = None
        trimerpos = None
        for i, pos in enumerate(enum_plaquette(cpos, w, h)):
            if (pos in newpos or pos in flipped) and prev_trimer != pos:
                index = i
                trimerpos = pos
                break
        else:
            break
        
        if pos in newpos:
            newpos.remove(trimerpos)
        else:
            flipped.remove(trimerpos)

        flipped.add(flip_trimer(cpos, index, w, h))
        prev_trimer = flip_trimer(cpos, index, w, h)
        cpos = flip_monomer(cpos, index, w, h)
    return list(newpos), list(flipped)

def show_worms(ax, positions, w, h):
    import collections
    Segment = collections.namedtuple("Segment", "loop type next dir")

    positions = set(positions)
    segments = {}
    loops = []

    UNKNOWN = 0
    LOOP = 1
    BRANCH = 2

    def start_worm(x, y):
        color = (x + 2 * y) % 3

        current_line = set()
        cpos = (x, y)
        while cpos not in segments:
            index = None
            trimerpos = None
            for i, pos in enumerate(enum_plaquette(cpos, w, h)):
                if pos in positions:
                    index = i
                    trimerpos = pos
                    break
            else:
                raise ValueError()
            
            current_line.add(cpos)
            next = flip_monomer(cpos, index, w, h)
            segments[cpos] = Segment(-1, UNKNOWN, next, index)
            cpos = next
        
        if segments[cpos].type == UNKNOWN:
            while cpos in current_line:
                prev = segments[cpos]
                segments[cpos] = Segment(len(loops), LOOP, prev.next, prev.dir)
                current_line.remove(cpos)
                cpos = prev.next
            for pos in current_line:
                prev = segments[pos]
                segments[pos] = Segment(len(loops), BRANCH, prev.next, prev.dir)

            loops.append({"start": cpos, "color": color})
        elif segments[cpos].type == BRANCH:
            loopid = segments[cpos].loop
            for pos in current_line:
                prev = segments[pos]
                segments[pos] = Segment(loopid, BRANCH, prev.next, prev.dir)
        elif segments[cpos].type == LOOP:
            loopid = segments[cpos].loop
            for pos in current_line:
                prev = segments[pos]
                segments[pos] = Segment(loopid, BRANCH, prev.next, prev.dir)

    for x in range(w):
        for y in range(h):
            # if any(trimer in positions for trimer in enum_starofdavid((x, y), w, h)):
            #     # it's not a vortex, skip
            #     continue
            if (x, y) in segments:
                continue

            start_worm(x, y)


    lines = [[], [], []]
    for begin, seg in segments.items():
        if seg.type != LOOP:
            continue
        
        color = (begin[0] + 2 * begin[1]) % 3
        end = seg.next
        if begin[0] == 0:
            if seg.dir in [1, 2, 3]:
                begin = begin[0] + w, begin[1]
        if end[0] == 0:
            if seg.dir in [4, 5, 0]:
                end = end[0] + w, end[1]
        if begin[1] == 0:
            if seg.dir in [3, 4, 5]:
                begin = begin[0], begin[1] + h
        if end[1] == 0:
            if seg.dir in [0, 1, 2]:
                end = end[0], end[1] + h
        

        half_offset = None
        if seg.dir == 1 and begin[1] == h - 1:
            half_offset = -0.5, 1
        elif seg.dir == 2 and begin[0] == 1:
            half_offset = -1, 0.5
        elif seg.dir == 4 and begin[1] == 1:
            half_offset = 0.5, -1
        elif seg.dir == 5 and begin[0] == w - 1:
            half_offset = 1, -0.5

        if half_offset is None:
            lines[color].append([mono_coords(*begin), mono_coords(*end)])
        else:
            half1 = begin[0] + half_offset[0], begin[1] + half_offset[1]
            half2 = end[0] - half_offset[0], end[1] - half_offset[1]
            lines[color].append([mono_coords(*begin), mono_coords(*half1)])
            lines[color].append([mono_coords(*half2), mono_coords(*end)])

    ax.add_collection(matplotlib.collections.LineCollection(lines[0], color="red", lw=2, ls="-", zorder=2))
    ax.add_collection(matplotlib.collections.LineCollection(lines[1], color="green", lw=2, ls="-", zorder=2))
    ax.add_collection(matplotlib.collections.LineCollection(lines[2], color="cyan", lw=2, ls="-", zorder=2))

    wx = collections.defaultdict(int)
    wy = collections.defaultdict(int)

    for x in range(w):
        s = segments[(x, 0)]
        if s.type == LOOP and s.dir in [0, 1, 2]:
            wx[s.loop] += 1
        s = segments[(x, h-1)]
        if s.type == LOOP and s.dir == 1:
            wx[s.loop] += 1

        s = segments[(x, 1)]
        if s.type == LOOP and s.dir in [3, 4, 5]:
            wx[s.loop] -= 1
        s = segments[(x, 2)]
        if s.type == LOOP and s.dir == 4:
            wx[s.loop] -= 1

    for y in range(h):
        s = segments[(0, y)]
        if s.type == LOOP and s.dir in [0, 4, 5]:
            wy[s.loop] += 1
        s = segments[(w-1, y)]
        if s.type == LOOP and s.dir == 5:
            wy[s.loop] += 1

        s = segments[(1, y)]
        if s.type == LOOP and s.dir in [1, 2, 3]:
            wy[s.loop] -= 1
        s = segments[(2, y)]
        if s.type == LOOP and s.dir == 2:
            wy[s.loop] -= 1

    for i in range(len(loops)):
        loops[i]["wx"] = wx[i]
        loops[i]["wy"] = wy[i]
    
    print(" ".join(str(("RGB"[loop["color"]], loop["wx"], loop["wy"])) for loop in loops))


def show_positions(ax, positions, type="worm", show_monomers=False, color="black", length=None, height=None, width=None, draw_triangular_lattice=True):
    def to_rgba(hex, alpha):
        return ((hex >> 16) / 256, ((hex >> 8) & 0xFF) / 256, (hex & 0xFF) / 256, alpha)

    color1 = 0x4dab2f
    color2 = 0xdecd2b
    color3 = 0xf78026
    color4 = 0xfd58f7
    color5 = 0x9c78fa
    color6 = 0x4db2f1
    colors = [color1, color2, color3, color4, color5, color6]

    if length is not None:
        width = length
        height= length

    if width is None:
        width = max(x for x, y, s in positions)+1

    if height is None:
        height = max(y for x, y, s in positions)+1

    if draw_triangular_lattice:
        draw_hexalattice(ax, width, height)

    positions = set(positions)

    monomers = set()
    for i in range(width):
        for j in range(height):
            monomers.add((i, j))
    doublons = set()

    nj4 = 0

    patches = []
    j4lines = []
    brokenj4lines = []
    facecolors = []
    updownfacecolors = []
    for x, y, s in positions:
        xy = trimer_coords(x, y, s)
        patches.append(mpatches.RegularPolygon(xy, numVertices=3,
                                            radius=1/np.sqrt(3),
                                            orientation=s*np.pi))
        sublattice = (4 * y + 2 * x + s) % 6
        facecolors.append(to_rgba(colors[sublattice], 1))
        updownfacecolors.append(to_rgba(colors[(sublattice % 2)*3+1], 1))
        
        mono_pos = []
        if s == 0:
            for dx, dy in [(0, 0), (1, 0), (0, 1)]:
                mono_pos.append(((x+dx)%width, (y+dy)%height))
        else:
            for dx, dy in [(1, 1), (1, 0), (0, 1)]:
                mono_pos.append(((x+dx)%width, (y+dy)%height))
        
        for mx, my in mono_pos:
            if (mx, my) in monomers:
                monomers.remove((mx, my))
            else:
                doublons.add((mx, my))

        if s == 0:
            for dx, dy in [(0, 1), (1, 0), (0, -2), (1, -2), (-2, 0), (-2, 1)]:
                if (x+dx, y+dy, 1) in positions:
                    j4lines.append([trimer_coords(x, y, 0), trimer_coords(x+dx, y+dy, 1)])
                    nj4 += 1
            for dx, dy in [(1, 1), (1, -2), (-2, 1)]:
                if (x+dx, y+dy, s) in positions:
                    brokenj4lines.append([trimer_coords(x, y, s), trimer_coords(x+dx, y+dy, s)])
        else:
            for dx, dy in [(1, 1), (1, -2), (-2, 1)]:
                if (x+dx, y+dy, s) in positions:
                    brokenj4lines.append([trimer_coords(x, y, s), trimer_coords(x+dx, y+dy, s)])
    
    monopatches = []
    for x, y in monomers:
        monopatches.append(mpatches.Circle(mono_coords(x, y), radius=1/np.sqrt(3)/2))
    doubpatches = []
    for x, y in doublons:
        doubpatches.append(mpatches.Circle(mono_coords(x, y), radius=1/np.sqrt(3)/2))

    print(nj4)

    if show_monomers:
        ax.add_collection(matplotlib.collections.PatchCollection(monopatches, edgecolors="black", facecolors="lime", zorder=1.5))
        ax.add_collection(matplotlib.collections.PatchCollection(doubpatches, edgecolors="black", facecolors="red", zorder=1.5))

    if type == "none":
        ax.add_collection(matplotlib.collections.PatchCollection(patches, edgecolors="black", facecolors=color, zorder=1.2))
    elif type == "worm":
        ax.add_collection(matplotlib.collections.PatchCollection(patches, edgecolors="black", facecolors=color, zorder=1.2))
        show_worms(ax, positions, width, height)
    elif type == "domain":
        ax.add_collection(matplotlib.collections.PatchCollection(patches, edgecolors=None, facecolors=facecolors, zorder=-1.2))
    elif type == "updown":
        ax.add_collection(matplotlib.collections.PatchCollection(patches, edgecolors=None, facecolors=updownfacecolors, zorder=-1.2))
    else:
        ax.add_collection(matplotlib.collections.PatchCollection(patches, edgecolors="black", facecolors=color, zorder=1.2))
        ax.add_collection(matplotlib.collections.LineCollection(brokenj4lines, color="orchid", lw=2, ls="-", zorder=2))
        ax.add_collection(matplotlib.collections.LineCollection(j4lines, color="dodgerblue", lw=2, ls="-", zorder=2))


    ax.axis("off")
    ax.set_xlim([0, 50])
    ax.set_ylim([0, 30])
    ax.set_aspect("equal")

def show_positions_dimer(ax, positions):
    positions = set(positions)

    lines = []
    for x, y, s in positions:
        if s == 0:
            lines.append([(x, y), (x+1, y)])
        else:
            lines.append([(x, y), (x, y+1)])
    ax.add_collection(matplotlib.collections.LineCollection(lines, color="black", lw=2, ls="-", zorder=2))

    ax.axis("off")
    ax.set_xlim([10, 50])
    ax.set_ylim([10, 30])
    ax.set_aspect("equal")

def autoshape(data, r=None):
    l = len(data)
    if r != None:
        root = int(round(np.sqrt(l / r)))
        if r == 1:
            return data.reshape((root, root)).transpose((1, 0))
        else:
            return data.reshape((root, root, r)).transpose((1, 0, 2))
    else:
        for r in range(1, 4):
            if l % r != 0:
                continue

            root = int(round(np.sqrt(l / r)))
            if root * root == l // r:
                if r == 1:
                    return data.reshape((root, root)).transpose((1, 0))
                else:
                    return data.reshape((root, root, r)).transpose((1, 0, 2))
    return None

def show_positions_dimer_hex(ax, positions):
    positions = set(positions)
    width = max(x for x, y, s in positions)+1
    height = max(y for x, y, s in positions)+1

    draw_hexalattice(ax, width, height)

    lines = []
    for x, y, s in positions:
        p1 = trimer_coords(x, y, 0)

        if s == 0:
            p2 = trimer_coords(x, y, 1)
        elif s == 1:
            p2 = trimer_coords(x-1, y, 1)
        elif s == 2:
            p2 = trimer_coords(x, y-1, 1)

        lines.append([p1, p2])
    ax.add_collection(matplotlib.collections.LineCollection(lines, color="black", lw=3, ls="-", zorder=2))

    ax.axis("off")
    ax.set_xlim([10, 50])
    ax.set_ylim([10, 30])
    ax.set_aspect("equal")

def read_accumulator(fname, skip=0, take=None):
    state = "count"
    total = 0

    n = 0
    mean = None
    m2 = None

    skipped = 0

    try:
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
    except Exception as e:
        print(fname)
        raise e

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
        l = int(tokens[0].split("x")[0])
        r = int(tokens[1][1:])
        if "_u" in basename:
            yield { "fname": fname, "l": l, "r": r, "n": (l**2-r)//3, "t": float(tokens[2][1:]), "u": float(tokens[3][1:]), "j4": float(tokens[4][1:]) }
        else:
            yield { "fname": fname, "l": l, "r": r, "n": (l**2-r)//3, "t": float(tokens[2][1:]), "j4": float(tokens[3][1:]) }

def get_all_data(glob, transform, filter=lambda _: True, skip=0, by="t", with_counts=False, take=None):
    ts0, ds0, ns = [], [], []
    for props in sorted(enum_files(glob), key=lambda x: x[by]):
        if not filter(props): continue

        try:
            data = read_accumulator(props["fname"], skip=skip, take=take)
        except ValueError:
            continue

        try:
            ds0.append(transform(data, props))
        except IndexError as e:
            print(e)
            continue

        ts0.append(props if by == "fname" else props[by])
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

def get_all_curves(globs, prop="cv", bounds=None, skip=0, take=None, by="l"):
    props = []
    ds = []

    for glob in globs:
        if prop == "cv":
            nprops, nds = get_all_cvs(glob, skip=skip, by="fname")
        elif prop == "e":
            nprops, nds = get_all_energies(glob, skip=skip, by="fname")
        elif prop == "b_e":
            nprops, nds = get_all_data(glob, lambda data, props: 1-data[0][4]/3/data[0][3]**2, skip=skip, by="fname")
        elif prop == "k":
            nprops, nds = get_all_data(glob, lambda data, props: data[0][3]/props["n"], skip=skip, by="fname")
        elif prop == "chi_k":
            nprops, nds = get_all_data(glob, lambda data, props: data[1][3]**2/props["t"]/props["n"], skip=skip, by="fname")
        elif prop == "b_k":
            nprops, nds = get_all_data(glob, lambda data, props: 1-data[0][5]/2/data[0][4]**2, skip=skip, by="fname")
        elif prop == "s":
            nprops, nds = get_all_data(glob, lambda data, props: data[0][8]/props["n"], skip=skip, by="fname")
        elif prop == "chi_s":
            nprops, nds = get_all_data(glob, lambda data, props: data[1][8]**2/props["t"]/props["n"], skip=skip, by="fname")
        elif prop == "b_s":
            nprops, nds = get_all_data(glob, lambda data, props: 1-data[0][10]/3/data[0][9]**2, skip=skip, by="fname")
        elif prop == "m":
            nprops, nds = get_all_data(glob, lambda data, props: data[0][0]/props["n"], skip=skip, by="fname")
        elif prop == "b_m":
            nprops, nds = get_all_data(glob, lambda data, props: 1-data[0][2]/2/data[0][1]**2, skip=skip, by="fname")
        elif prop == "mono":
            nprops, nds = get_all_data(glob, lambda data, props: data[0][0]/props["n"], skip=skip, by="fname")
        elif prop == "corr":
            nprops, nds = get_all_data(glob, lambda data, props: (1/2)*np.sqrt(data[0][3]/data[0][6] - 1), skip=skip, by="fname")
        elif prop == "corr2":
            nprops, nds = get_all_data(glob, lambda data, props: (1/2)*np.sqrt((data[0][6]/data[0][7] - 1)/(4 - data[0][6]/data[0][7])), skip=skip, by="fname")

        props += nprops.tolist()
        ds += nds.tolist()

    lens = sorted(set(x[by] for x in props))

    curves = []
    for l in lens:
        ts = []
        ys = []
        for prop, y in sorted(((prop, y) for prop, y in zip(props, ds) if prop[by] == l), key=lambda x: x[0]["t"]):
            if bounds is None or bounds[0] < prop[by] < bounds[1]:
                ts.append(prop["t"])
                ys.append(y)
        curves.append((l, np.array(ts), np.array(ys)))
    return curves

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

def plot2d(ax, data, log=False, show_dimer=False, vmax=None):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []

    if vmax is None:
        vmax = np.amax(data)

    if log:
        data = data + 0.000001
        N = matplotlib.colors.LogNorm(vmin=np.amin(data), vmax=vmax)
    else:
        N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=vmax)

    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            colors.append(matplotlib.colormaps["magma"](N(data[i, j])))
            if i >= data.shape[0]//2:
                i -= data.shape[0]
            if j >= data.shape[1]//2:
                j -= data.shape[1]
            patches.append(matplotlib.patches.RegularPolygon(mono_coords(i, j), numVertices=6, radius=1/np.sqrt(3), orientation=0))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    if show_dimer:
        ax.add_patch(mpatches.Rectangle((-0.25, -0.25), 1.5, 0.5, ec=None, fc="white", zorder=1))

    ax.set_xlabel("$x$ (lattice constants)")
    ax.set_ylabel("$y$ (lattice constants)")

    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="magma"), ax=ax)

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

                patches.append(matplotlib.patches.RegularPolygon(trimer_coords(i, j, k), numVertices=3, radius=1/np.sqrt(3), orientation=k*np.pi))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))

    ax.set_xlabel("$x$")
    ax.set_ylabel("$y$")

    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def plot2d_hexdimer(ax, data, title=None, skew=False):
    ax.axis("off")
    ax.set_xlim([-25, 25])
    ax.set_ylim([-15, 15])
    ax.set_aspect("equal")

    patches = []
    colors = []
    N = matplotlib.colors.Normalize(vmin=np.min(data), vmax=np.max(data))

    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            for k in range(3):
                colors.append(matplotlib.colormaps["viridis"](N(data[i, j, k])))
                i2, j2 = i, j
                
                if j2 >= data.shape[1]//2:
                    j2 -= data.shape[1]
                    if skew:
                        i2 -= data.shape[1]
                if i2 >= data.shape[0]//2:
                    i2 -= data.shape[0]

                xy = mono_coords(i2, j2)

                patches.append(matplotlib.patches.Rectangle(xy, np.sqrt(3)/3, 0.4, angle=30+k*120, rotation_point=(xy[0], xy[1] + 0.2)))

    ax.add_collection(matplotlib.collections.PatchCollection(patches, facecolors=colors))
    plt.colorbar(matplotlib.cm.ScalarMappable(norm=N, cmap="viridis"), ax=ax)
    plt.title(title)

def FTpos(pos):
    newx = []
    newy = []

    def add(x, y, s):
        x0 = trimer_coords(x, y, s)
        newx.append(x0[0])
        newy.append(x0[1])

    for p in pos:
        add(*p)

    newx = np.array(newx)
    newy = np.array(newy)

    def corr(k):
        return np.sum(np.exp(-1j * (newx * k[0] * np.pi + newy * k[1] * np.pi)))
    return corr

def FT(vals, coords=trimer_coords, matrix=False):
    newvals = []
    newx = []
    newy = []

    radius = vals.shape[0]/4
    if matrix:
        sqrts = int(round(np.sqrt(vals.shape[2])))

    def add(x, y, s):
        s0 = 0
        s1 = s
        if matrix:
            s0 = s // sqrts
            s1 = s % sqrts

        x0 = coords(0, 0, s0)
        xy = coords(x, y, s1)

        xy = xy[0]-x0[0], xy[1]-x0[1]

        if (xy[0]**2 + xy[1]**2 < (radius*4)**2):
            newx.append(xy[0])
            newy.append(xy[1])
            newvals.append(vals[x%vals.shape[0], y%vals.shape[1], s] * np.exp(-(xy[0]**2+xy[1]**2)/(2*radius**2)))

    bounding = int(vals.shape[0]*2.5)
    for i in range(bounding):
        for j in range(bounding):
            for s in range(vals.shape[2]):
                add(i-bounding//2, j-bounding//2, s)

    newx = np.array(newx)
    newy = np.array(newy)
    newvals = np.array(newvals)

    def corr(k):
        return np.sum(np.exp(-1j * (newx * k[0] * np.pi + newy * k[1] * np.pi)) * newvals)
    return corr

coords = {}
def make_coords(extent):
    global coords
    if extent in coords:
        return

    X = np.linspace(extent[0], extent[1], 400)
    Y = np.linspace(extent[2], extent[3], 400)

    coords[extent] = np.zeros((len(X), len(Y), 2))
    for x in range(coords[extent].shape[0]):
        for y in range(coords[extent].shape[1]):
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

            coords[extent][x, y] = [xy[0], xy[1]]

def plot_FT(ax, ft, proj="re", fold=True, vmax=None, res=75, extent=2.5, draw_extra=True):
    if not isinstance(extent, tuple):
        extent = (-extent, extent, -extent, extent)

    make_coords(extent)

    if fold:
        datares = res
        dataX = np.linspace(-0.02, extent[1]*1.01, datares * 2)
        dataY = np.linspace(-0.02, extent[1]*1.01/np.sqrt(3), datares)
        data = np.zeros((datares * 2, datares), dtype=complex)
        for x in range(datares * 2):
            for y in range(min(datares, x//2 + 3)):
                data[x, y] = ft((dataX[x], dataY[y]))
        import scipy
        interp = scipy.interpolate.RegularGridInterpolator((dataX, dataY), data)
        interped = interp(coords[extent])
    else:
        datares = res
        dataX = np.linspace(extent[0], extent[1], datares*2)
        dataY = np.linspace(extent[2], extent[3], datares*2)
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
    elif proj == "abs":
        mapped = np.abs(interped)
        if vmax is None:
            vmax = np.amax(mapped)
        norm = matplotlib.colors.Normalize(vmin=0, vmax=vmax)
        cmap = "viridis"
    elif proj == "absre":
        mapped = np.abs(np.real(interped))
        if vmax is None:
            vmax = np.amax(mapped)
        norm = matplotlib.colors.Normalize(vmin=0, vmax=vmax)
        cmap = "viridis"
    elif proj == "logabs":
        mapped = np.abs(interped) + 0.0001
        if vmax is None:
            vmax = np.amax(mapped)
        norm = matplotlib.colors.LogNorm(vmin=np.amin(mapped), vmax=vmax)
        cmap = "viridis"
    elif proj == "logre":
        mapped = np.abs(np.real(interped)) + 0.0001
        if vmax is None:
            vmax = np.amax(mapped)
        norm = matplotlib.colors.LogNorm(vmin=np.amin(mapped), vmax=vmax)
        cmap = "viridis"


    ax.imshow(mapped.T, origin="lower", extent=extent, cmap=cmap, norm=norm)
    ax.grid(False)
    ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/3, fill=None, ec="w", orientation=np.pi/6, alpha=0.5, lw=1))
    ax.text(0, 0.1, "$\\Gamma$", color="w", horizontalalignment="center", size=13).set_clip_on(True)
    if draw_extra:
        ax.text(4/3, 0.1, "$K$", color="w", size=13).set_clip_on(True)
        ax.text(-4/3, 0.1, "$K'$", horizontalalignment="right", color="w", size=13).set_clip_on(True)
        ax.add_patch(mpatches.RegularPolygon((0, 0), 6, radius=4/np.sqrt(3), fill=None, ec="w", alpha=0.5, lw=1))
    # ax.text(0, 1.25, "$M$", horizontalalignment="center", color="w", size=13).set_clip_on(True)

    ax.set_xlabel("$\\pi k_x$", fontsize=12)
    ax.set_ylabel("$\\pi k_y$", fontsize=12)

    plt.colorbar(matplotlib.cm.ScalarMappable(norm, cmap=cmap), ax=ax)

def plot_timeseries(ax, data, std, label=None):
    xs = np.arange(len(data))
    ax.plot(xs, data, label=label)
    ax.fill_between(xs, data-std, data+std, alpha=0.4)