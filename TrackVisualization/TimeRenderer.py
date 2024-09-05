from Parameters import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

particle_index = 0
z, gen_t, gen_x, gen_y, gen_v, gen_xz, gen_yz, pre_t, pre_st, pre_x, pre_sx, pre_y, pre_sy, pre_v, pre_sv, pre_xz, pre_sxz, pre_yz, pre_syz, fil_t, fil_st, fil_x, fil_sx, fil_y, fil_sy,fil_v, fil_sv, fil_xz, fil_sxz, fil_yz, fil_syz, smo_t, smo_st, smo_x, smo_sx, smo_y, smo_sy, smo_v, smo_sv, smo_xz, smo_sxz, smo_yz, smo_syz = np.loadtxt(f"../data/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)

fig = plt.figure()
ax: Axes3D = fig.add_subplot(111, projection='3d')

square_x = np.array([gen_x.min(), gen_x.max()])
square_t = np.array([gen_t.min(), gen_t.max()])
x, t = np.meshgrid(square_x, square_t)
for z_value in z:
    zz = np.full_like(x, z_value)
    ax.plot_surface(zz, x, t, color=(0.1, 0.2, 0.5, 0.2))
    ax.plot_wireframe(zz, x, t)

ax.set_xlabel("z [m]")
ax.set_ylabel("x [m]")
ax.set_zlabel("t [s]")

ax.plot(z[1:], fil_x[1:], fil_t[1:], ls=":", color="blue")
ax.plot(z, smo_x, smo_t, ls="-.", color="green")
ax.scatter(z, gen_x, gen_t, marker="o", color="black", label="generated")
ax.scatter(z[1:], fil_x[1:], fil_t[1:], marker=".", color="blue", label="filtered")
ax.scatter(z[3:], pre_x[3:], pre_t[3:], marker="o", color="red", label="predicted")
ax.scatter(z, smo_x, smo_t, marker=".", color="green", label="smoothed")

ax.legend()

plt.show()
fig.savefig(f"Time3D.pdf")
