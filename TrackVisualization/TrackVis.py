import numpy as np
import matplotlib
import matplotlib.pyplot as plt

PARTICLE_LIST = range(1203, 1207)
PARTICLE_LIST = [1200,1201,1202,1210]
DETECTOR_SPACE_UNCERTAINTY = 1e-6;
DETECTOR_TIME_UNCERTAINTY = 1e-11;

for particle_index in PARTICLE_LIST:
    data = np.loadtxt(f"../results/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
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
    ylabels = [r"$t\, [\text{ns}]$", r"$x\, [\text{μm}]$", r"$y\, [\text{μm}]$", r"$1/v_z\, \left[\text{s}/\text{m}\right]$", r"$v_x/v_z\, \left[\right]$", r"$v_y/v_z\, \left[\right]$"]
    yfilenames = ["t", "x", "y", "v", "xz", "yz"]

    smoothed_initial_index = {"t": 1, "x": 1, "y": 1, "v": 1, "xz": 1, "yz": 1}
    filtered_initial_index = {"t": 1, "x": 1, "y": 1, "v": 1, "xz": 1, "yz": 1}
    scale_factor = {"t": 1e9, "x": 1e6, "y": 1e6, "v": 1, "xz": 1, "yz": 1}

    for (y_t, y_r, y_m, y_f, y_p, y_s, sy_m, sy_f, sy_p, sy_s, ylabel, yfilename) in zip(ys_t, ys_r, ys_m, ys_f, ys_p, ys_s, sys_m, sys_f, sys_p, sys_s, ylabels, yfilenames):
        figure, ax = plt.subplots()
        ax.grid()
        ax.set_xlabel(r"$z\, [\text{m}]$", fontsize=12)
        ax.ticklabel_format(style="sci",axis="y", scilimits=(-2,3), useMathText=True)
        ax.set_ylabel(ylabel, fontsize=12)
        # z = z*1e2
        s = smoothed_initial_index[yfilename]
        f = filtered_initial_index[yfilename]
        ax.plot(z, y_t*scale_factor[yfilename], label="theoretical", color="orange")
        ax.errorbar(z, y_r*scale_factor[yfilename], fmt=".:", elinewidth=1.5, capsize=2.5, label="real", color="black")
        if y_m.size != 0:
            ax.errorbar(z[1:], y_m[1:]*scale_factor[yfilename], yerr=sy_m*scale_factor[yfilename], ls=" ", fmt="o", elinewidth=1.5, capsize=2.5, label="measured", color="grey")
        ax.errorbar(z[2:], y_p[2:]*scale_factor[yfilename], yerr=sy_p[2:]*scale_factor[yfilename], fmt="o", elinewidth=1.5, capsize=2.5, label="predicted", color="red")
        ax.errorbar(z[f:], y_f[f:]*scale_factor[yfilename], yerr=sy_f[f:]*scale_factor[yfilename], fmt="o-.", elinewidth=1.5, capsize=2.5, label="filtered", color="blue")
        ax.errorbar(z[s:], y_s[s:]*scale_factor[yfilename], yerr=sy_s[s:]*scale_factor[yfilename], fmt="o:", elinewidth=1.5, capsize=2.5, label="smoothed", color="green")
        ax.legend(fontsize=12)
        figure.savefig(f"figures/Particle {particle_index} {yfilename}.pdf", bbox_inches="tight")
        plt.close(figure)
