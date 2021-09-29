"""
Plot function being minimized, along with iterative solutions
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm 
from mpl_toolkits.mplot3d import Axes3D 
import matplotlib.animation

def update_scatter(num):
    print(num)
    """
    data_x=5.*np.random.rand(10)
    data_y=5.*np.random.rand(10)
    data_z=5.*np.random.rand(10)
    """
    scatter._offsets3d = (data_x[num], data_y[num], data_z[num])
    #title.set_text('3D Test, time={}'.format(num))

# Read HISTORY_ALL
fh = open('HISTORY_ALL', 'r')
data_x = []
data_y = []
data_z = []
for g in range(0,101):
    line = fh.readline()
    data_x_g = []
    data_y_g = []
    data_z_g = []
    for i in range(0,20):
        line = fh.readline()
        nums = [float(x) for x in line.split()]
        data_x_g.append(nums[0])
        data_y_g.append(nums[1])
        data_z_g.append(nums[2])
    data_x.append(data_x_g)
    data_y.append(data_y_g)
    data_z.append(data_z_g)
data_x = np.array(data_x)
data_y = np.array(data_y)
data_z = np.array(data_z)
#print(data_x)
 
X = np.linspace(-5.12, 5.12, 500)     
Y = np.linspace(-5.12, 5.12, 500)     
X, Y = np.meshgrid(X, Y) 

Z = (X**2 - 10 * np.cos(2 * np.pi * X)) + \
  (Y**2 - 10 * np.cos(2 * np.pi * Y)) + 20
 
fig = plt.figure() 
ax = fig.gca(projection='3d') 

ax.plot_surface(X, Y, Z,
  #cmap=cm.seismic,
  #cmap=cm.gnuplot,
  #cmap=cm.twilight_shifted,
  #cmap=cm.winter,
  #cmap=cm.viridis,
  #cmap=cm.RdGy,
  #cmap=cm.gist_gray,
  cmap=cm.gist_earth,
  #cmap=cm.inferno,
  alpha = 0.1,
  shade=True,
  antialiased=False)   

"""
data_x=5.*np.random.rand(10)
data_y=5.*np.random.rand(10)
data_z=5.*np.random.rand(10)
"""
scatter = ax.scatter(data_x[0], data_y[0], data_z[0],color="black")

ani = matplotlib.animation.FuncAnimation(fig, update_scatter, 100, interval=100, blit=False)

ani.save('test.gif')

#plt.show()
