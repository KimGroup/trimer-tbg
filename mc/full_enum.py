import pocket
import util
pocket.width = 12
pocket.height = 12

def topo_sectors(pos, lines, l):
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
                    cpos = (cpos[0] - 1, cpos[1], 1)
                else:
                    cpos = (cpos[0] - 1, cpos[1] + 1, 0)
            elif dir == 2:
                if cpos[2] == 0:
                    cpos = (cpos[0], cpos[1] - 1, 1)
                else:
                    cpos = (cpos[0] + 1, cpos[1] - 1, 0)

            cpos = (cpos[0] % l, cpos[1] % l, cpos[2])

            if cpos == start:
                break
        vals.append(total - l//3)
    return vals

it = iter(pocket.enumerate_tilings())
n = 0
import collections
d1 = collections.defaultdict(int)

with open("new-data/-1_0_0_-2_pos_12x12exact.dat", "w") as f:
    try:
        while True:
            pos = next(it)
            # secs = topo_sectors(pocket.rowspace_to_trimers(pos), [((0, 0, 0), 0), ((2, 0, 0), 1), ((1, 0, 0), 2), ((1, 0, 0), 0), ((0, 0, 0), 1), ((2, 0, 0), 2), ((2, 0, 0), 0), ((1, 0, 0), 1), ((0, 0, 0), 2)], pocket.width)
            secs = topo_sectors(pocket.rowspace_to_trimers(pos), [((0, 0, 0), 0), ((2, 0, 0), 1), ((1, 0, 0), 0), ((0, 0, 0), 1)], pocket.width)

            # i+j=A
            # -2i+j=B
            # i = (A-B)/3
            # j = (2A+B)/3
            ri, rj = (secs[0]-secs[1])//3, (2*secs[0]+secs[1])//3
            gi, gj = (secs[2]-secs[3])//3, (2*secs[2]+secs[3])//3

            d1[(ri, rj, gi, gj)] += 1
            if (ri, rj, gi, gj) == (-1, 0, 0, -2):
                f.write(util.conf_to_string(pocket.rowspace_to_trimers(pos)) + "\n")
                f.flush()

            n += 1
            if n % 10000 == 0:
                print(d1)
    except StopIteration:
        pass

print(d1)
import pickle
with open("exacttopo12.dat", "wb") as f:
    pickle.dump(d1, f)
