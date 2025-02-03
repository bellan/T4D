from matplotlib.patches import Circle
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import art3d

def data_for_cylinder_along_z(center_x,center_y,center_z,radius,height_z):
    z = np.linspace(center_z, center_z+height_z, 50)
    theta = np.linspace(0, 2*np.pi, 50)
    theta_grid, z_grid=np.meshgrid(theta, z)
    x_grid = radius*np.cos(theta_grid) + center_x
    y_grid = radius*np.sin(theta_grid) + center_y
    return x_grid,y_grid,z_grid

def ceiling_cylinder(center_x,center_y,center_z,radius):
    z = np.array([center_z])
    rad = np.linspace(0, radius, 50)
    theta = np.linspace(0, 2*np.pi, 50)
    theta_grid, z_grid=np.meshgrid(theta, z)
    x_grid = radius*np.cos(theta_grid) + center_x
    y_grid = radius*np.sin(theta_grid) + center_y
    return x_grid,y_grid,z_grid

particle_shape = ["o", "v", "s", "*", "s", "+", "D"]
particle_color = ["black", "blue", "green", "red", "orange", "purple", "brown"]
particle_index = [1202, 1203, 1205, 1206, 1207, 1208, 1209]
run_index = 0
xs = []
ys = []
zs = []
max = 0
for particle in particle_index:
    extraction = np.loadtxt(f"../results/Run {run_index} Particle {particle}.csv", skiprows=2, unpack=True, delimiter=",", dtype=float)
    z,_,_,_,_,_,_,_,x,y,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ = extraction
    x *= 1e3
    y *= 1e3
    max = np.max([max, (np.abs(x)).max(), (np.abs(y)).max()])
    xs.append(x[1:])
    ys.append(y[1:])
    zs.append(z[1:]*1e2)

fig = plt.figure()
ax: Axes3D = fig.add_subplot(111, projection='3d')

Xc,Yc,Zc = data_for_cylinder_along_z(0.,0.,-0.5,0.05,0.5)
ax.plot_surface(Zc, Xc, Yc, alpha=1, color="gray")
floor = Circle((0.,0.), 0.05, color="black")
ax.add_patch(floor)
art3d.pathpatch_2d_to_3d(floor, z=0, zdir="x")
floor = Circle((0.,0.), 0.05, color="black")
ax.add_patch(floor)
art3d.pathpatch_2d_to_3d(floor, z=-0.5, zdir="x")

max *= 1.2
square_x = np.array([-max, max])
square_y = square_x.copy()
mesh_x, mesh_y = np.meshgrid(square_x, square_y)
for z_value in zs[0]:
    zz = np.full_like(mesh_x, z_value)
    ax.plot_surface(zz, mesh_x, mesh_y, color=(0.1, 0.2, 0.5, 0.1))
    ax.plot_wireframe(zz, mesh_x, mesh_y, color="gray")

ax.set_xlabel("z [cm]")
ax.set_ylabel("x [mm]")
ax.set_zlabel("y [mm]")

for (z,x,y,shape,color) in zip(zs, xs,ys,particle_shape,particle_color):
    ax.scatter(z, x, y, marker=shape, color=color)
# ax.plot(z, fil_x, fil_y, ls=":", color="blue")
# ax.plot(z, smo_x, smo_y, ls="-.", color="green")
# ax.scatter(z, gen_x, gen_y, marker="o", color="black", label="generated")
# ax.scatter(z, fil_x, fil_y, marker=".", color="blue", label="filtered")
# ax.scatter(z[3:], pre_x[3:], pre_y[3:], marker="o", color="red", label="predicted")
# ax.scatter(z, smo_x, smo_y, marker=".", color="green", label="smoothed")

# ax.legend()

plt.show()
fig.savefig(f"../figures/TrackVisualization/3D.pdf")
