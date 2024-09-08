from Parameters import *
import numpy as np
import matplotlib.pyplot as plt

# for particle_index in range(PARTICLE_NUMBER):
for particle_index in [0]:
    data = np.loadtxt(f"../results/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    z, rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz, mes_t, mes_x, mes_y, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = data
    # rea_v = 1./rea_v
    # fil_v[2:] = 1./fil_v[2:]
    # pre_v[3:] = 1./pre_v[3:]
    # smo_v = 1./smo_v
    #
    # fil_sv[2:] = fil_sv[2:]/(fil_v[2:]*fil_v[2:])
    # pre_sv[3:] = pre_sv[3:]/(pre_v[3:]*pre_v[3:])
    # smo_sv = smo_sv/(smo_v*smo_v)

    void_array = np.array([])

    ys_r = [rea_t, rea_x, rea_y]
    ys_m = [mes_t, mes_x, mes_y]
    ys_s = [smo_t, smo_x, smo_y]
    sys_m = [DETECTOR_TIME_UNCERTAINTY, DETECTOR_SPACE_UNCERTAINTY, DETECTOR_SPACE_UNCERTAINTY]
    sys_s = [smo_st, smo_sx, smo_sy]
    ylabels = ["t [s]", "x [m]", "y [m]"]
    yfilenames = ["t", "x", "y"]

    smoothed_initial_index = {"t": 1, "x": 1, "y": 1}

    for (y_r, y_m, y_s, sy_m, sy_s, ylabel, yfilename) in zip(ys_r, ys_m, ys_s, sys_m, sys_s, ylabels, yfilenames):
        figure, ax = plt.subplots()
        ax.grid()
        ax.set_xlabel("z [m]")
        ax.set_ylabel(ylabel)
        s = smoothed_initial_index[yfilename]
        mask = np.full_like(y_s, True, dtype=bool)
        mask[5+1] = False
        antimastk = np.invert(mask)
        mask[0]=False
        # ax.errorbar(z, y_t, ls=" ", fmt="o", elinewidth=1, capsize=1, label="theoretical", color="yellow")
        ax.errorbar(z, y_r, fmt=".:", elinewidth=1, capsize=1, label="real", color="black")
        if y_m.size != 0:
            ax.errorbar(z[1:], y_m[1:], yerr=sy_m, ls=" ", fmt="o", elinewidth=1, capsize=1, label="measured", color="grey")
        ax.errorbar(z[mask], y_s[mask], yerr=sy_s[mask], fmt=".:", elinewidth=1, capsize=1, label="smoothed", color="green")
        ax.errorbar(z[antimastk], y_s[antimastk], yerr=sy_s[antimastk], fmt="o", elinewidth=1, capsize=1, label="smoothed", color="red")
        ax.legend()
        figure.savefig(f"figures/Particle {particle_index} {yfilename}.pdf")
        plt.close(figure)
