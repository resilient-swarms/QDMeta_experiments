RASTRI_DIM=2 #0
import numpy as np
import pickle
from mpl_toolkits.mplot3d import Axes3D
def rastrigin(x):
    sum = 10.0 * RASTRI_DIM
    A = 10.0
    for i in range(RASTRI_DIM):
        sum += (x[i] * x[i] - A * np.cos(2 * np.pi * x[i]))
    return -sum


def batches(n_batch,size_per_batch,dim=18):
    for i in range(n_batch):
        X = np.random.rand(size_per_batch,dim)
        pickle.dump(X,open("rastri_batch" + str(i), "wb"))


# Axes3D import has side effects, it enables using projection='3d' in add_subplot
import matplotlib.pyplot as plt
import random

def evaluate_rastrigin_dimensiontest(x):
    sum = 10.0 * (RASTRI_DIM - 1)
    A = 10.0f;
    for i in range(RASTRI_DIM):
        if i==1:
            print("ignoring dimension ",i)
            sum -= A *  np.sin(6.0 * np.pi * x[i])
            continue
        sum += (x[i] * x[i] - A * np.cos(2 * np.pi * x[i]));
    return -sum

def evaluate_rastrigin_dimension(x):
    sum = 10.0 * (RASTRI_DIM - 1)
    A = 10.0
    for i in range(RASTRI_DIM):
        if i==1:
            print("ignoring dimension ",i)
            sum -= A *  np.sin(6.0 * np.pi * x[i])
            continue
        sum += (x[i] * x[i] - A * np.cos(2 * np.pi * x[i]));
    return -sum

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
x = y = np.arange(-3.0, 3.0, 0.05)
X, Y = np.meshgrid(x, y)
z1s = np.array(rastrigin())
Z1 = z1s.reshape(X.shape)

ax.plot_surface(X, Y, Z)

ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

plt.show()

if __name__ == "__main__":
    batches(10,100000)
