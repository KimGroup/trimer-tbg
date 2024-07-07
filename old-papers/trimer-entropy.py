import numpy as np
import matplotlib.pyplot as plt
import scipy.integrate
import scipy.integrate as si


def get(b):
    def t(z, c):
        """
        c: 1 for l, -1 for r
        """
        k = ((z-1/z) - b) / ((z-1/z) - np.conj(b))
        return np.power(np.abs(k), 1/6) * \
            np.exp(1j * (np.angle(k) + 2 * np.pi * c) / 6)

    """
    # plot branch cuts in z plane

    def argk(z):
        return np.angle(((z-1/z) - b) / ((z-1/z) - np.conj(b)))

    RE, IM = np.meshgrid(np.linspace(-2, 2, 500), np.linspace(-2, 2, 500))
    phase = argk(RE + 1j * IM)
    plt.imshow(phase[:, ::-1], cmap="hsv", vmin=0, vmax=2*np.pi, lims=[-2, 2, -2, 2])
    plt.show()
    """

    def fl(z):
        return (t(z, 1) + 1/t(z, 1)) / z

    def fr(z):
        return (t(z, -1) + 1/t(z, -1)) / z

    imb = np.imag(b)

    # "type II" homology
    bl = (b + np.sqrt(b * b + 4)) / 2
    br = (b - np.sqrt(b * b + 4)) / 2

    # semicircular path to right of L branch cut
    def imfl(z):
        return np.imag(fl(np.real(bl) + np.imag(bl) * np.exp(1j * z)) * np.imag(bl) * 1j * np.exp(1j * z))
    # mirror

    def imfr(z):
        return np.imag(fr(np.real(br) + np.imag(br) * np.exp(1j * z)) * np.imag(br) * 1j * np.exp(1j * z))

    # path direction matters
    rho_l = 1 / 2 / np.pi * si.quad(imfl, -np.pi / 2, np.pi / 2)[0]
    rho_r = 1 / 2 / np.pi * si.quad(imfr, np.pi / 2, 3 * np.pi / 2)[0]

    # semicircular path above both branch cuts
    def refl(z):
        return np.real(fl(1j * np.imag(bl) + np.real(bl) * np.exp(1j * z)) * np.real(bl) * 1j * np.exp(1j * z))

    def refr(z):
        return np.real(fr(1j * np.imag(br) + -np.real(br) * np.exp(1j * z)) * -np.real(br) * 1j * np.exp(1j * z))

    phi_l = 1 / 2 * si.quad(refl, 0, np.pi)[0]
    phi_r = 1 / 2 * si.quad(refr, np.pi, 0)[0]

    ib = np.imag(bl)
    rb = np.real(bl)

    def flm1(z):
        return (t(z, 1) + 1/t(z, 1) - 1) / z

    def frm1(z):
        return (t(z, -1) + 1/t(z, -1) - 1) / z
    # vertical path to complex infinity

    def reflm1_o(z):
        return np.real(flm1(1j*z) * 1j)

    def refrm1_o(z):
        return np.real(frm1(1j*z) * 1j)

    # integrand is numerically singular at 0
    sigma_l = 1 / 4 * si.quad(reflm1_o, 0.00001, np.inf)[0]
    sigma_r = 1 / 4 * si.quad(refrm1_o, 0.00001, np.inf)[0]

    dPhi_dphil = 1/6 * (-rho_l + 2 * rho_r)
    dPhi_dphir = 1/6 * (2 * rho_l - rho_r)

    rho_0 = (-3/2 + 1/2 * rho_r) * dPhi_dphil + (-3/2 + 1/2 *
                                                 rho_l) * dPhi_dphir - 1/4 * rho_l - 1/4 * rho_r + 1
    rho_1 = (1/2 - 1/2 * rho_r) * dPhi_dphil + (3/2 - 1/2 * rho_l) * \
        dPhi_dphir - 1/4 * rho_l + 1/4 * rho_r
    rho_2 = (1/2 + 1/2 * rho_r) * dPhi_dphil + (-1/2 + 1/2 *
                                                rho_l) * dPhi_dphir - 1/4 * rho_l + 1/4 * rho_r

    rho_3 = (-1/2 - 1/2 * rho_r) * dPhi_dphil + (-1/2 - 1/2 *
                                                 rho_l) * dPhi_dphir + 1/4 * rho_l + 1/4 * rho_r
    rho_4 = (-1/2 + 1/2 * rho_r) * dPhi_dphil + (1/2 + 1/2 *
                                                 rho_l) * dPhi_dphir + 1/4 * rho_l - 1/4 * rho_r
    rho_5 = (3/2 - 1/2 * rho_r) * dPhi_dphil + (1/2 - 1/2 * rho_l) * \
        dPhi_dphir + 1/4 * rho_l - 1/4 * rho_r

    S = sigma_l + sigma_r + dPhi_dphil * phi_l + dPhi_dphir * phi_r

    return (rho_1 + rho_3 + rho_5, S)


p = []
s = []
for b in np.linspace(0.001, 1.999, 50):
    P, S = get(b * 1j)
    p.append(P)
    s.append(S)

print(np.log(np.sum(np.exp(np.array(s)))/2))
print(s)

p, s = np.array(p), np.array(s)

# symmetry of up/down
plt.subplots(1, 1, figsize=[3, 2])
plt.plot(np.concatenate((p, 1-p[::-1])), np.concatenate((s, s[::-1])))
plt.xlabel("$\\rho_{AB}=\\frac{n_{AB}}{n_{AB}+n_{BA}}$")
plt.ylabel("S/N")
plt.tight_layout()
plt.xlim([0, 1])
plt.ylim([0, 0.3])
plt.savefig("entropy.png", dpi=200)
plt.show()
