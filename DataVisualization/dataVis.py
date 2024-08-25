from Parameters import *
import numpy as np
import matplotlib.pyplot as plt

for particle_index in range(PARTICLE_NUMBER):
    data = np.loadtxt(f"../data/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    z, the_t, the_x, the_y, the_v, the_xz, the_yz, rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz, mes_t, mes_x, mes_y, pre_t, pre_st, pre_x, pre_sx, pre_y, pre_sy, pre_v, pre_sv, pre_xz, pre_sxz, pre_yz, pre_syz, fil_t, fil_st, fil_x, fil_sx, fil_y, fil_sy,fil_v, fil_sv, fil_xz, fil_sxz, fil_yz, fil_syz, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = data
    # rea_v = 1./rea_v
    # fil_v[2:] = 1./fil_v[2:]
    # pre_v[3:] = 1./pre_v[3:]
    # smo_v = 1./smo_v
    #
    # fil_sv[2:] = fil_sv[2:]/(fil_v[2:]*fil_v[2:])
    # pre_sv[3:] = pre_sv[3:]/(pre_v[3:]*pre_v[3:])
    # smo_sv = smo_sv/(smo_v*smo_v)

    void_array = np.array([])

    ys_t = [the_t, the_x, the_y, the_v, the_xz, the_yz]
    ys_r = [rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz]
    ys_m = [mes_t, mes_x, mes_y, void_array, void_array, void_array]
    ys_f = [fil_t, fil_x, fil_y, fil_v, fil_xz, fil_yz]
    ys_p = [pre_t, pre_x, pre_y, pre_v, pre_xz, pre_yz]
    ys_s = [smo_t, smo_x, smo_y, smo_v, smo_xz, smo_yz]
    sys_m = [DETECTOR_TIME_UNCERTAINTY, DETECTOR_SPACE_UNCERTAINTY, DETECTOR_SPACE_UNCERTAINTY, void_array, void_array, void_array]
    sys_f = [fil_st, fil_sx, fil_sy, fil_sv, fil_sxz, fil_syz]
    sys_p = [pre_st, pre_sx, pre_sy, pre_sv, pre_sxz, pre_syz]
    sys_s = [smo_st, smo_sx, smo_sy, smo_sv, smo_sxz, smo_syz]
    ylabels = ["t [s]", "x [m]", "y [m]", "1/(v_z) [s/m]", "(v_x/v_z) []", "(v_y/v_z) []"]
    yfilenames = ["t", "x", "y", "v", "xz", "yz"]

    smoothed_initial_index = {"t": 1, "x": 1, "y": 1, "v": 2, "xz": 2, "yz": 2}
    filtered_initial_index = {"t": 1, "x": 1, "y": 1, "v": 2, "xz": 2, "yz": 2}

    for (y_t, y_r, y_m, y_f, y_p, y_s, sy_m, sy_f, sy_p, sy_s, ylabel, yfilename) in zip(ys_t, ys_r, ys_m, ys_f, ys_p, ys_s, sys_m, sys_f, sys_p, sys_s, ylabels, yfilenames):
        figure, ax = plt.subplots()
        ax.grid()
        ax.set_xlabel("z [m]")
        ax.set_ylabel(ylabel)
        s = smoothed_initial_index[yfilename]
        f = filtered_initial_index[yfilename]
        # ax.errorbar(z, y_t, ls=" ", fmt="o", elinewidth=1, capsize=1, label="theoretical", color="yellow")
        ax.plot(z, y_t, label="theoretical", color="yellow")
        ax.errorbar(z, y_r, fmt=".:", elinewidth=1, capsize=1, label="real", color="black")
        if y_m.size != 0:
            ax.errorbar(z, y_m, yerr=sy_m, ls=" ", fmt="o", elinewidth=1, capsize=1, label="measured", color="grey")
        ax.errorbar(z[3:], y_p[3:], yerr=sy_p[3:], fmt="o", elinewidth=1, capsize=1, label="predicted", color="red")
        ax.errorbar(z[f:], y_f[f:], yerr=sy_f[f:], fmt=".-.", elinewidth=1, capsize=1, label="filtered", color="blue")
        ax.errorbar(z[s:], y_s[s:], yerr=sy_s[s:], fmt=".:", elinewidth=1, capsize=1, label="smoothed", color="green")
        ax.legend()
        figure.savefig(f"Particle {particle_index} {yfilename}.pdf")
        plt.close(figure)
