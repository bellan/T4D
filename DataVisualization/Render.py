from Parameters import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

particle_index = 1
z, gen_t, gen_x, gen_y, gen_v, gen_xz, gen_yz, pre_t, pre_st, pre_x, pre_sx, pre_y, pre_sy, pre_v, pre_sv, pre_xz, pre_sxz, pre_yz, pre_syz, fil_t, fil_st, fil_x, fil_sx, fil_y, fil_sy,fil_v, fil_sv, fil_xz, fil_sxz, fil_yz, fil_syz, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = np.loadtxt(f"../data/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

square_x = np.array([gen_x.min(), gen_x.max()])
square_y = np.array([gen_y.min(), gen_y.max()])
x, y = np.meshgrid(square_x, square_y)
for z_value in z:
    zz = np.full_like(x, z_value)
    ax.plot_surface(x, y, zz, color=(0.1, 0.2, 0.5, 0.3))
    ax.plot_wireframe(x, y, zz)


ax.plot(fil_x, fil_y, z, ls=":", color="blue")
ax.plot(smo_x, smo_y, z, ls="-.", color="green")
ax.scatter(gen_x, gen_y, z, marker="o", color="black", label="generated")
ax.scatter(fil_x, fil_y, z, marker=".", color="blue", label="filtered")
ax.scatter(pre_x, pre_y, z, marker="o", color="red", label="predicted")
ax.scatter(smo_x, smo_y, z, marker=".", color="green", label="smoothed")

ax.legend()

plt.show()
fig.savefig(f"3D.pdf")
