import matplotlib.pyplot as plt
import numpy as np
from matplotlib.widgets import Slider, Button

# model: H = (e + aVB/r - VG) n


def didv_dot(vb, vg, x, a, e):
    if a == 0:
        elevels = [e, e - vg]
    else:
        elevels = [e, e + a * vb / np.abs(x) - vg]

    gap = abs(elevels[0] - elevels[1])

    return 3/(1 + ((gap)/0.03)**2) + \
        0

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
tetra_pos = [(0, 1), (0, 0), (-np.sqrt(3)/2, -0.5), (np.sqrt(3)/2, -0.5)]

lattice_pos = []
for i in range(-1, 1):
    for j in range(-1, 1):
        lattice_pos.append((i + j/2, j * np.sqrt(3)/2))

def didv_multi(positions, xy, local_gate_potential_type, interaction_potential_type, bias_factor, site_bias, params=None):
    import itertools
    eigens = []

    for occup in itertools.product([0, 1], repeat=len(positions)):
        occs = tuple(occup)
        if sum(occs) < len(positions) - 3:
            continue

        tote = 0

        if local_gate_potential_type == "point":
            for pos, occ in zip(positions, occs):
                if occ > 0:
                    tote += site_bias + bias_factor / np.sqrt(0.05**2 + np.hypot(pos[0] - xy[0], pos[1] - xy[1])**2)
        elif local_gate_potential_type == "fidget":
            for pos, occ in zip(positions, occs):
                if occ > 0:
                    if pos[1] != 0:
                        tote += site_bias + bias_factor / 3 * (1 / np.sqrt(0.05**2 + np.hypot(pos[0] - xy[0], pos[1]+1 - xy[1])**2)
                            + 1 / np.sqrt(0.05**2 + np.hypot(pos[0]+np.sqrt(3)/2 - xy[0], pos[1]-0.5 - xy[1])**2)
                            + 1 / np.sqrt(0.05**2 + np.hypot(pos[0]-np.sqrt(3)/2 - xy[0], pos[1]-0.5 - xy[1])**2))
                    else:
                        tote += site_bias + bias_factor / 3 * (1 / np.sqrt(0.05**2 + np.hypot(pos[0] - xy[0], pos[1]-1 - xy[1])**2)
                            + 1 / np.sqrt(0.05**2 + np.hypot(pos[0]+np.sqrt(3)/2 - xy[0], pos[1]+0.5 - xy[1])**2)
                            + 1 / np.sqrt(0.05**2 + np.hypot(pos[0]-np.sqrt(3)/2 - xy[0], pos[1]+0.5 - xy[1])**2))
        else:
            raise ValueError()

        if interaction_potential_type == "point":
            for i in range(len(occs)):
                for j in range(i+1, len(occs)):
                    if occs[i] > 0 and occs[j] > 0:
                        tote += params["Unn"]
        elif interaction_potential_type == "fidget":
            for i in range(len(occs)):
                for j in range(i+1, len(occs)):
                    if (i == 1 or j == 1) and occs[i] > 0 and occs[j] > 0:
                        tote += params["Unn"]
        else:
            raise ValueError()

        eigens.append((tote, sum(occs)))

    eigens.sort(key=lambda x: x[0])
    gs = eigens[0][0]
    gsnum = eigens[0][1]
    for energy, num in eigens:
        if num != gsnum:
            gap = energy - gs
            break

    # local gate contribution
    return 3/(1 + ((gap)/0.03)**2) + \
        0
    # sum(
    #     # dos with lorentzian widening
    #     1/(1 + ((vb - energy)/0.1)**2)
    #     for energy, num in eigens
    #     # if abs(num - gsnum) == 1
    # )


### SINGLE DOT BIAS AND POSITION SCAN
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


### SINGLE DOT BIAS AND GATE SCAN
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

### TRIPLE DOT POSITION SCAN
plt.figure()
x = np.linspace(0, 1, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(vb)):
    for j in range(len(x)):
        didvs[i, j] = didv_multi(
            T_pos,
            (0, -x[j] + 0.5),
            "point",
            "point",
            bias_factor=vb[i],
            site_bias=0.5-Vg,
            params={'Unn': 0.3}
        )

plt.imshow(didvs.T, extent=[0, 2, 0, 1], origin="lower")
plt.xlabel("$V_b$")
plt.ylabel("x")

### DOUBLE DOT POSITION SCAN
plt.figure()
x = np.linspace(0, 1, 200)
vb = np.linspace(0, 2, 200)
didvs = np.zeros((200, 200))
for i in range(len(vb)):
    for j in range(len(x)):
        didvs[i, j] = didv_multi(
            D_pos,
            (0, x[j] - 0.5),
            "point",
            "point",
            bias_factor=vb[i],
            site_bias=0.5-Vg,
            params={'Unn': 0.3}
        )

plt.imshow(didvs.T, extent=[0, 2, 0, 1], origin="lower")
plt.xlabel("$V_b$")
plt.ylabel("x")


def plotT():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 2, -2, 2],
                    origin="lower", vmin=0, vmax=4)
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
        x = np.linspace(-2, 2, 100)
        y = np.linspace(-2, 2, 100)
        didvs = np.zeros((100, 100))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    T_pos,
                    (x[i], y[j]),
                    "point",
                    "point",
                    bias_factor=val,
                    site_bias=0.5-Vg,
                    params={'Unn': 0.3}
                )

        im.set_data(didvs.T)

    update(1)

    freq_slider.on_changed(update)
    return freq_slider


def plotD():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 2, -2, 2],
                    origin="lower", vmin=0, vmax=4)
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
        x = np.linspace(-2, 2, 100)
        y = np.linspace(-2, 2, 100)
        didvs = np.zeros((100, 100))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    D_pos,
                    (x[i], y[j]),
                    "point",
                    "point",
                    bias_factor=val,
                    site_bias=0.5-Vg,
                    params={'Unn': 0.3}
                )

        im.set_data(didvs.T)

    update(1)

    freq_slider.on_changed(update)
    return freq_slider

def plotLattice():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 1, -1.5, 1],
                    origin="lower", vmin=0, vmax=4)
    plt.xlabel("x")
    plt.ylabel("y")

    axfreq = fig.add_axes([0.25, 0.1, 0.65, 0.03])
    freq_slider = Slider(
        ax=axfreq,
        label='Vb',
        valmin=0,
        valmax=1,
        valinit=0.5,
    )

    def update(val):
        x = np.linspace(-2, 1, 60)
        y = np.linspace(-1.5, 1, 60)
        didvs = np.zeros((60, 60))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    lattice_pos,
                    (x[i], y[j]),
                    "point",
                    "point",
                    bias_factor=val,
                    site_bias=0.2-Vg,
                    params={'Unn': 0.1}
                )

        im.set_data(didvs.T)

    update(0.5)

    freq_slider.on_changed(update)
    return freq_slider

def plotFidget():
    fig = plt.figure()
    fig.subplots_adjust(bottom=0.25)
    didvs = np.zeros((100, 100))
    im = plt.imshow(didvs.T, extent=[-2, 2, -2, 2],
                    origin="lower", vmin=0, vmax=4)
    plt.xlabel("x")
    plt.ylabel("y")

    axfreq = fig.add_axes([0.25, 0.1, 0.65, 0.03])
    freq_slider = Slider(
        ax=axfreq,
        label='Vb',
        valmin=0,
        valmax=2,
        valinit=0.5,
    )

    def update(val):
        x = np.linspace(-2, 2, 100)
        y = np.linspace(-2, 2, 100)
        didvs = np.zeros((100, 100))
        for i in range(len(x)):
            for j in range(len(y)):
                didvs[i, j] = didv_multi(
                    tetra_pos,
                    (x[i], y[j]),
                    "fidget",
                    "fidget",
                    bias_factor=val,
                    site_bias=0.5-Vg,
                    params={'Unn': 0.2}
                )

        im.set_data(didvs.T)

    update(0.5)

    freq_slider.on_changed(update)
    return freq_slider

# keep = plotT()
keep2 = plotD()
keep3 = plotLattice()
keep4 = plotFidget()

plt.show()
