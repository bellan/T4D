import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

particle_index = 0
extraction = np.loadtxt(f"../results/Particle {particle_index}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
z, _, _, _, _, _, _, gen_t, gen_x, gen_y, _, _, _, _, _, _, pre_t, _, pre_x, _, pre_y, _, pre_v, _, _, _, _, _, fil_t, _, fil_x, _, fil_y, _, _, _, _, _, _, _, smo_t, _, smo_x, _, smo_y, _, _, _, _, _, _, _ = extraction

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
