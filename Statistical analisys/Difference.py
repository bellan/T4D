import numpy as np
import matplotlib.pyplot as plt

analyzed_detector_index = 5 # NOTE: Detector indexing start at 0
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
for particle_index in range(PARTICLE_NUMBER):
    if (particle_index % 100 == 0):
        print(particle_index)
    data = np.loadtxt(f"../results/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    _, _, _, _, _, _, _, rea_t, rea_x, rea_y, rea_v, rea_xz, rea_yz, mes_t, mes_x, mes_y, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = data
    mes_t_dif.append(mes_t[analyzed_detector_index] - rea_t[analyzed_detector_index])
    mes_x_dif.append(mes_x[analyzed_detector_index] - rea_x[analyzed_detector_index])
    mes_y_dif.append(mes_y[analyzed_detector_index] - rea_y[analyzed_detector_index])
    smo_t_dif.append(smo_t[analyzed_detector_index] - rea_t[analyzed_detector_index])
    smo_x_dif.append(smo_x[analyzed_detector_index] - rea_x[analyzed_detector_index])
    smo_y_dif.append(smo_y[analyzed_detector_index] - rea_y[analyzed_detector_index])
    if (abs(mes_t[analyzed_detector_index] - rea_t[analyzed_detector_index])>0.75e-10):
        print(f"AAAAAAAAA {particle_index}")

mes_t_dif = np.array(mes_t_dif)
mes_x_dif = np.array(mes_x_dif)
mes_y_dif = np.array(mes_y_dif)
smo_t_dif = np.array(smo_t_dif)
smo_x_dif = np.array(smo_x_dif)
smo_y_dif = np.array(smo_y_dif)


ys = [(mes_t_dif, smo_t_dif), (mes_x_dif, smo_x_dif), (mes_y_dif, smo_y_dif)]
names = ["t", "x", "y"]
for (y,name) in zip(ys,names):
    figure, ax = plt.subplots()
    ax.grid()
    measured, smoothed = y
    ax.hist(measured, label="measured", bins=20)
    ax.hist(smoothed, label="smoothed", bins=20)
    ax.set_xlabel("Differenc")
    ax.set_ylabel("Occurrences")
    ax.legend()
    figure.savefig(f"figures/{name}.pdf")
    plt.close(figure)
