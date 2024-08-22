from Parameters import *
import numpy as np
import matplotlib.pyplot as plt

for particle_index in range(PARTICLE_NUMBER):
    z, gen_t, gen_x, gen_y, gen_v, gen_xz, gen_yz, mes_t, mes_x, mes_y, pre_t, pre_st, pre_x, pre_sx, pre_y, pre_sy, pre_v, pre_sv, pre_xz, pre_sxz, pre_yz, pre_syz, fil_t, fil_st, fil_x, fil_sx, fil_y, fil_sy,fil_v, fil_sv, fil_xz, fil_sxz, fil_yz, fil_syz, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = np.loadtxt(f"../data/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)

    # gen_v = 1./gen_v
    # fil_v[2:] = 1./fil_v[2:]
    # pre_v[3:] = 1./pre_v[3:]
    # smo_v = 1./smo_v
    #
    # fil_sv[2:] = fil_sv[2:]/(fil_v[2:]*fil_v[2:])
    # pre_sv[3:] = pre_sv[3:]/(pre_v[3:]*pre_v[3:])
    # smo_sv = smo_sv/(smo_v*smo_v)

    void_array = np.array([])

    ys_g = [gen_t, gen_x, gen_y, gen_v, gen_xz, gen_yz]
    ys_m = [mes_t, mes_x, mes_y, void_array, void_array, void_array]
    ys_f = [fil_t, fil_x, fil_y, fil_v, fil_xz, fil_yz]
    ys_p = [pre_t, pre_x, pre_y, pre_v, pre_xz, pre_yz]
    ys_s = [gen_t, gen_x, gen_y, gen_v, gen_xz, gen_yz]
    sys_f = [fil_st, fil_sx, fil_sy, fil_sv, fil_sxz, fil_syz]
    sys_p = [pre_st, pre_sx, pre_sy, pre_sv, pre_sxz, pre_syz]
    sys_s = [smo_st, smo_sx, smo_sy, smo_sv, smo_sxz, smo_syz]
    ylabels = ["t [s]", "x [m]", "y [m]", "1/(v_z) [s/m]", "(v_x/v_z) []", "(v_y/v_z) []"]
    yfilenames = ["t", "x", "y", "v", "xz", "yz"]

    smoothed_initial_index = {"t": 0, "x": 1, "y": 0, "v": 1, "xz": 2, "yz": 2}

    for (y_g, y_m, y_f, y_p, y_s, sy_f, sy_p, sy_s, ylabel, yfilename) in zip(ys_g, ys_m, ys_f, ys_p, ys_s, sys_f, sys_p, sys_s, ylabels, yfilenames):
        figure, ax = plt.subplots()
        ax.grid()
        ax.set_xlabel("z [m]")
        ax.set_ylabel(ylabel)
        s = smoothed_initial_index[yfilename]
        ax.errorbar(z, y_g, ls=" ", fmt="o", elinewidth=1, capsize=1, label="generated", color="black")
        if y_m.size != 0:
            ax.errorbar(z, y_m, ls=" ", fmt="o", elinewidth=1, capsize=1, label="measured", color="grey")
        ax.errorbar(z[3:], y_p[3:], yerr=sy_p[3:], fmt="o", elinewidth=1, capsize=1, label="predicted", color="red")
        ax.errorbar(z[2:], y_f[2:], yerr=sy_f[2:], fmt=".-.", elinewidth=1, capsize=1, label="filtered", color="blue")
        ax.errorbar(z[s:], y_s[s:], yerr=sy_s[s:], fmt=".:", elinewidth=1, capsize=1, label="smoothed", color="green")
        ax.legend()
        figure.savefig(f"Particle {particle_index} {yfilename}.pdf")
        plt.close(figure)
