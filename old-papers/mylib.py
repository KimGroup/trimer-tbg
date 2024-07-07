import matplotlib.pyplot as plt
import matplotlib
import numpy as np


hcenters = {}
for i in range(-10, 1000):
    for j in range(-10, 1000):
        x = i * (2 - (1 - np.cos(np.pi / 3)))
        y = j * np.sin(np.pi / 3) * 2
        if i % 2 == 0:
            y += np.sin(np.pi / 3)
        hcenters[(i, j)] = (x, y)


def make_hex(ij, **kwargs):
    t = np.linspace(0, 2 * np.pi, 7)
    xy = np.stack((hcenters[ij][0] + np.cos(t), hcenters[ij][1] + np.sin(t)), axis=1)
    return matplotlib.patches.Polygon(xy, closed=True, **kwargs)

def get_trimer_xy(ij, d):
    if d == 0:
        if ij[0] % 2 == 1:
            xy = np.array([hcenters[ij], hcenters[(ij[0] - 1, ij[1] - 1)], hcenters[(ij[0] - 1, ij[1])]])
        else:
            xy = np.array([hcenters[ij], hcenters[(ij[0] - 1, ij[1])], hcenters[(ij[0] - 1, ij[1] + 1)]])
    else:
        if ij[0] % 2 == 1:
            xy = np.array([hcenters[ij], hcenters[(ij[0] + 1, ij[1] - 1)], hcenters[(ij[0] + 1, ij[1])]])
        else:
            xy = np.array([hcenters[ij], hcenters[(ij[0] + 1, ij[1])], hcenters[(ij[0] + 1, ij[1] + 1)]])
    return xy

def pull_in_trimer(xy, e):
    mean = np.mean(xy, axis=0)
    xy = (xy - np.tile(mean, [3, 1])) * e + np.tile(mean, [3, 1])
    return xy

def make_trimer(ij, d, **kwargs):   
    return matplotlib.patches.Polygon(get_trimer_xy(ij, d), closed=True, **kwargs)

def color_trimer(hcolors, ij, d, color, **kwargs):
    if d == 0:
        if ij[0] % 2 == 1:
            l = [ij, (ij[0] - 1, ij[1] - 1), (ij[0] - 1, ij[1])]
        else:
            l = [ij, (ij[0] - 1, ij[1]), (ij[0] - 1, ij[1] + 1)]
    else:
        if ij[0] % 2 == 1:
            l = [ij, (ij[0] + 1, ij[1] - 1), (ij[0] + 1, ij[1])]
        else:
            l = [ij, (ij[0] + 1, ij[1]), (ij[0] + 1, ij[1] + 1)]
    
    for x in l:
        hcolors[x] = color
        
def make_stripe(x, t, fc, bc, spacing=0):
    trimer_list = []
    
    if x % 2 == 1:
        for i in range(5):
            trimer_list.append(((x, t - 2 + i * (3 + spacing)), 1, fc, bc))
            trimer_list.append(((x + 1, t - 1 + i * (3 + spacing)), 0, fc, bc))
    else:
        for i in range(5):
            trimer_list.append(((x, t - 2 + i * (3 + spacing)), 1, fc, bc))
            trimer_list.append(((x + 1, t + i * (3 + spacing)), 0, fc, bc))
        
        
    return trimer_list

def draw(l):
    hcolors = {}

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    for i, j, fc, bc in l:
        color_trimer(hcolors, i, j, bc)

    for i in range(-5, 20):
        for j in range(-5, 20):
            if (i, j) in hcolors:
                ax.add_patch(make_hex((i, j), fc=hcolors[(i, j)], ec='black'))
            else:
                ax.add_patch(make_hex((i, j), fill=False))

    for i, j, fc, bc in l:
        ax.add_patch(make_trimer(i, j, fc=fc))

    plt.axis('equal')
    ax.set_xlim([0, 20])
    ax.set_ylim([0, 20])
    
def calculate_coulomb(xy):
    total = 0
    for i in range(len(xy)):
        for j in range(i + 1, len(xy)):
            total += 1 / np.linalg.norm(xy[i] - xy[j])
    return total