import nnfs
import nnfs.datasets
import numpy as np
import matplotlib.pyplot as plt

nnfs.init()

X,y = nnfs.datasets.spiral_data(samples=100, classes=3)

plt.scatter(X[:,0], X[:, 1], c=y, cmap='brg')
plt.show()  