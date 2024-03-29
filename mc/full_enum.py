import pocket
import util
pocket.width = 3
pocket.height = 4

it = iter(pocket.enumerate_tilings())
n = 0
import collections
d1 = collections.defaultdict(int)

try:
    while True:
        pos = next(it)
        sector = util.winding_numbers(pos, pocket.width, pocket.width)

        d1[sector] += 1
        if sector != (0, 0, 0, 0):
            pass

        # with open(f"new-data/exact-sectors-12x12/{sector[0]}_{sector[1]}_{sector[2]}_{sector[3]}.dat", "a") as f:
        with open(f"new-data/3x4pos.dat", "a") as f:
            f.write(util.conf_to_string(pos) + "\n")
            f.flush()

        n += 1
        if n % 10000 == 0:
            print(d1)
except StopIteration:
    pass

print(sum(d1.values()))

# import pickle
# with open("exacttopo12.dat", "wb") as f:
#     pickle.dump(d1, f)
