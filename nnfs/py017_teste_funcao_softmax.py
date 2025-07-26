from py015_funcoes_de_ativacao import *
from py012_classe_camada import *
import nnfs
import nnfs.datasets
import numpy as np

nnfs.init()

X,y = nnfs.datasets.spiral_data(samples=100, classes=3)

dense1 = Dense_Layer(qtd_inputs=2, qtd_neurons=3)
dense1.forward(inputs=X)

activation1 = Activation_ReLU()
activation1.forward(inputs=dense1.outputs)

dense2 = Dense_Layer(3, 3)
dense2.forward(inputs=dense1.outputs)

activation2 = Activation_Softmax()
activation2.forward(inputs=dense2.outputs)

print(activation2.outputs[:5])