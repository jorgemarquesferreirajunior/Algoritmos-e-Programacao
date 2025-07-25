from py012_classe_camada import *
import nnfs
import nnfs.datasets
import numpy as np

nnfs.init()

X,y = nnfs.datasets.spiral_data(samples=100, classes=3)

dense1 = Dense_Layer(qtd_inputs=2, qtd_neurons=3)
dense1.forward(inputs=X)

print(dense1.outputs[:5])
