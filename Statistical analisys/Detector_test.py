import numpy as np
import matplotlib.pyplot as plt

det_idx = 5 # NOTE: Detector indexing start at 0
det_idx+=1 # NOTE: Because the first must be ignore since is the initial state
PARTICLE_NUMBER = 10000

FOUNDAMENTAL_CHARGE = 1.602176634e-19;
LIGHT_SPEED = 299792458.;

MIN_PARTICLE_MASS = (0.5e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);
MAX_PARTICLE_MASS = (300e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);

DETECTOR_SPACE_UNCERTAINTY = 1e-6;
DETECTOR_TIME_UNCERTAINTY = 1e-11;

mes_t_dif = []
mes_x_dif = []
mes_y_dif = []
smo_t_dif = []
smo_x_dif = []
smo_y_dif = []

ext_vz_dif = []
ext_vx_dif = []
ext_vy_dif = []
smo_vz_dif = []
smo_vx_dif = []
smo_vy_dif = []

pull_t = []
pull_x = []
pull_y = []

pull_mesmo_t = []

for particle_index in range(PARTICLE_NUMBER):
    if (particle_index % 100 == 0):
        print(particle_index)
    data = np.loadtxt(f"../results/Detector test part {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    z, rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz, mes_t, mes_x, mes_y, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = data
    mes_t_dif.append(mes_t[det_idx] - rea_t[det_idx])
    mes_x_dif.append(mes_x[det_idx] - rea_x[det_idx])
    mes_y_dif.append(mes_y[det_idx] - rea_y[det_idx])
    smo_t_dif.append(smo_t[det_idx] - rea_t[det_idx])
    smo_x_dif.append(smo_x[det_idx] - rea_x[det_idx])
    smo_y_dif.append(smo_y[det_idx] - rea_y[det_idx])

    ext_vz_dif.append((mes_t[det_idx] - mes_t[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_v[det_idx])
    ext_vx_dif.append((mes_x[det_idx] - mes_x[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_xz[det_idx])
    ext_vy_dif.append((mes_y[det_idx] - mes_y[det_idx-1])/(z[det_idx]-z[det_idx-1]) - rea_yz[det_idx])
    smo_vz_dif.append(smo_v[det_idx] - rea_v[det_idx])
    smo_vx_dif.append(smo_xz[det_idx] - rea_xz[det_idx])
    smo_vy_dif.append(smo_yz[det_idx] - rea_yz[det_idx])

    pull_t.append((smo_t[det_idx] - rea_t[det_idx])/smo_st[det_idx])
    pull_x.append((smo_x[det_idx] - rea_x[det_idx])/smo_sx[det_idx])
    pull_y.append((smo_y[det_idx] - rea_y[det_idx])/smo_sy[det_idx])

    pull_mesmo_t.append((smo_t[det_idx] - mes_t[det_idx])/np.sqrt(DETECTOR_TIME_UNCERTAINTY**2+smo_st[det_idx]**2))

mes_t_dif = np.array(mes_t_dif)
mes_x_dif = np.array(mes_x_dif)
mes_y_dif = np.array(mes_y_dif)
smo_t_dif = np.array(smo_t_dif)
smo_x_dif = np.array(smo_x_dif)
smo_y_dif = np.array(smo_y_dif)

ext_vz_dif = np.array(ext_vz_dif)
ext_vx_dif = np.array(ext_vx_dif)
ext_vy_dif = np.array(ext_vy_dif)
smo_vz_dif = np.array(smo_vz_dif)
smo_vx_dif = np.array(smo_vx_dif)
smo_vy_dif = np.array(smo_vy_dif)

pull_t = np.array(pull_t)
pull_x = np.array(pull_x)
pull_y = np.array(pull_y)

pull_mesmo_t = np.array(pull_mesmo_t)


ys = [(mes_t_dif, smo_t_dif), (mes_x_dif, smo_x_dif), (mes_y_dif, smo_y_dif), (ext_vz_dif, smo_vz_dif), (ext_vx_dif, smo_vx_dif), (ext_vy_dif, smo_vy_dif)]
names = ["res_t", "res_x", "res_y", "res_vz", "res_xz", "res_yz"]
for (y,name) in zip(ys,names):
    figure, ax = plt.subplots()
    ax.grid()
    measured, smoothed= y
    _, bin_edges, _ = ax.hist(measured, label="measured", bins=50)
    bin_size = bin_edges[1] - bin_edges[0]
    new_bin_number = int((smoothed.max() - smoothed.min())/bin_size)
    ax.hist(smoothed, label="smoothed", alpha=0.8, bins=new_bin_number)
    ax.set_xlabel("Difference")
    ax.set_ylabel("Occurrences")
    ax.legend()
    figure.savefig(f"figures/Detector test {name}.pdf")
    plt.close(figure)


ys = [pull_t, pull_x, pull_y, pull_mesmo_t]
names = ["pull_t", "pull_x", "pull_y", "pull_mesmo"]
for (y,name) in zip(ys,names):
    figure, ax = plt.subplots()
    x = np.linspace(pull_x.min(), pull_x.max(), 1000)
    ax.grid()
    values, bin_edges, _ = ax.hist(y, label="measured", bins=50)
    area = ((bin_edges[1:]-bin_edges[:-1])*values).sum()
    ax.plot(x,  area/np.sqrt(2*np.pi) * np.exp(-0.5*x**2) )
    ax.set_xlabel("Difference")
    ax.set_ylabel("Occurrences")
    ax.legend()
    figure.savefig(f"figures/Detector test {name}.pdf")
    plt.close(figure)
