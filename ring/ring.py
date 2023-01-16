import matplotlib.pyplot as plt
import numpy as np
from matplotlib.widgets import Slider, Button

# model: H = (e + aVB/r - VG) n


def didv_dot(vb, vg, x, a, e):
    if a == 0:
        elevels = [e, e - vg]
    else:
        elevels = [e, e + a * vb / np.abs(x) - vg]

    return sum(
        # gaussian orbital for qdot
        # (np.exp(-x**2/2)/np.sqrt(2*np.pi)) *
        # dos with lorentzian widening
        1/(1 + ((vb - elevels[level])/0.1)**2)
        for level in range(len(elevels))
    )


D_pos = [(0, 0.5), (0, -0.5)]
T_pos = [(0, 0.5), (0.5/2*np.sqrt(3), -0.5/2),
         (-0.5/2*np.sqrt(3), -0.5/2)]


def didv_multi(positions, vb, vg, xy, a, e, unn):
    import itertools
    eigens = []

    # for i in range(len(positions)):
    #     occs = [0] * len(positions)
    #     occs[i] = 1
    for occup in itertools.product([0, 1], repeat=len(positions)):
        occs = tuple(occup)
        tote = 0
        for pos, occ in zip(positions, occs):
            if occ > 0:
                tote += e + (a * vb / np.sqrt(0.05**2 + np.hypot(pos[0] - xy[0], pos[1] - xy[1])**2)) \
                    - vg
        for i in range(len(occs)):
            for j in range(len(occs)):
                tote += unn/2 * occs[i] * occs[j]

        eigens.append((tote, sum(occs)))

    eigens.sort(key=lambda x: x[0])
    gs = eigens[0][0]
    gsnum = eigens[0][1]
    for energy, num in eigens:
        if num != gsnum:
            gap = energy - gs
            break

    # local gate contribution
    return 3/(1 + ((gap)/0.1)**2) + \
        0
    # sum(
    #     # dos with lorentzian widening
    #     1/(1 + ((vb - energy)/0.1)**2)
    #     for energy, num in eigens
    #     # if abs(num - gsnum) == 1
    # )


plt.figure()
x = np.linspace(-2, 2, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(x)):
    for j in range(len(vb)):
        didvs[i, j] = didv_dot(vb[j], 1, x[i], 1, 0.5)

plt.xlabel("r")
plt.ylabel("$V_b$")
plt.imshow(didvs.T, extent=[-2, 2, 0, 2], origin="lower")


plt.figure()
vg = np.linspace(0, 2, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(vb)):
    for j in range(len(vg)):
        didvs[i, j] = didv_dot(vb[i], vg[j], 0.5, 1, 0.5)

plt.imshow(didvs.T, extent=[0, 2, 0, 2], origin="lower")
plt.xlabel("$V_b$")
plt.ylabel("$V_g$")

Vg = 1.5
Unn = 0.3
alpha = 1


plt.figure()
x = np.linspace(0, 1, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(vb)):
    for j in range(len(x)):
        didvs[i, j] = didv_multi(
            T_pos, vb[i], Vg, (0, -x[j] + 0.5), 1, 0.5, Unn)

plt.imshow(didvs.T, extent=[0, 2, 0, 1], origin="lower")
plt.xlabel("$V_b$")
plt.ylabel("x")

plt.figure()
x = np.linspace(0, 1, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(vb)):
    for j in range(len(x)):
        didvs[i, j] = didv_multi(
            D_pos, vb[i], Vg, (0, x[j] - 0.5), 1, 0.5, Unn)

plt.imshow(didvs.T, extent=[0, 2, 0, 1], origin="lower")
plt.xlabel("$V_b$")
plt.ylabel("x")


def plotT():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 2, -2, 2],
                    origin="lower", vmin=0, vmax=2)
    plt.xlabel("x")
    plt.ylabel("y")

    axfreq = fig.add_axes([0.25, 0.1, 0.65, 0.03])
    freq_slider = Slider(
        ax=axfreq,
        label='Vb',
        valmin=0,
        valmax=4,
        valinit=1,
    )

    def update(val):
        print("updateT")
        x = np.linspace(-2, 2, 100)
        y = np.linspace(-2, 2, 100)
        didvs = np.zeros((100, 100))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    T_pos, val, Vg, (x[i], y[j]), 1, 0.5, Unn)
        im.set_data(didvs.T)

    freq_slider.on_changed(update)
    return freq_slider


def plotD():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 2, -2, 2],
                    origin="lower", vmin=0, vmax=2)
    plt.xlabel("x")
    plt.ylabel("y")

    axfreq = fig.add_axes([0.25, 0.1, 0.65, 0.03])
    freq_slider = Slider(
        ax=axfreq,
        label='Vb',
        valmin=0,
        valmax=4,
        valinit=1,
    )

    def update(val):
        print("update")
        x = np.linspace(-2, 2, 100)
        y = np.linspace(-2, 2, 100)
        didvs = np.zeros((100, 100))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    D_pos, val, Vg, (x[i], y[j]), 1, 0.5, Unn)
        im.set_data(didvs.T)

    freq_slider.on_changed(update)
    return freq_slider


keep = plotT()
keep2 = plotD()


plt.show()
