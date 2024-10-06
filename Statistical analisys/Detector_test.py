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

pull_mesmo_t = []
pull_mesmo_x = []
pull_mesmo_y = []

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

    pull_mesmo_t.append((smo_t[det_idx] - mes_t[det_idx])/np.sqrt(DETECTOR_TIME_UNCERTAINTY**2+smo_st[det_idx]**2))
    pull_mesmo_x.append((smo_x[det_idx] - mes_x[det_idx])/np.sqrt(DETECTOR_SPACE_UNCERTAINTY**2+smo_sx[det_idx]**2))
    pull_mesmo_y.append((smo_y[det_idx] - mes_y[det_idx])/np.sqrt(DETECTOR_SPACE_UNCERTAINTY**2+smo_sy[det_idx]**2))

mes_t_dif = np.array(mes_t_dif)
mes_x_dif = np.array(mes_x_dif)
mes_y_dif = np.array(mes_y_dif)
smo_t_dif = np.array(smo_t_dif)
smo_x_dif = np.array(smo_x_dif)
smo_y_dif = np.array(smo_y_dif)

pull_mesmo_t = np.array(pull_mesmo_t)
pull_mesmo_x = np.array(pull_mesmo_x)
pull_mesmo_y = np.array(pull_mesmo_y)


ys = [(mes_t_dif, smo_t_dif), (mes_x_dif, smo_x_dif), (mes_y_dif, smo_y_dif)]
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


ys = [pull_mesmo_t, pull_mesmo_x, pull_mesmo_y]
names = ["pull_mesmo_t", "pull_mesmo_x", "pull_mesmo_y"]
for (y,name) in zip(ys,names):
    figure, ax = plt.subplots()
    x = np.linspace(-1*(pull_mesmo_x.min()+pull_mesmo_x.max())*0.5, (pull_mesmo_x.min()+pull_mesmo_x.max())*0.5, 1000)
    ax.grid()
    values, bin_edges, _ = ax.hist(y, label=r"$\frac{x_t - x_r}{\sqrt{\sigma_t^2 + \sigma_r^2}}$", bins=50)
    area = ((bin_edges[1:]-bin_edges[:-1])*values).sum()
    ax.plot(x,  area/np.sqrt(2*np.pi) * np.exp(-0.5*x**2), label=r"$y=\frac{A}{\sqrt{2\pi}}e^{-\frac{x^2}{2}}$" )
    ax.set_ylabel("Occurrences")
    ax.legend()
    figure.savefig(f"figures/Detector test {name}.pdf")
    plt.close(figure)
