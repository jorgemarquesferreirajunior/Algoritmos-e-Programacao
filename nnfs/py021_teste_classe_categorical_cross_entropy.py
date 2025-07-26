from py012_classe_camada import *
from py015_funcoes_de_ativacao import *
from py019_classe_categorical_cross_entropy import *
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

loss_function = Loss_CategoricalCrossEntropy()
loss = loss_function.calculate(y_pred=activation2.outputs, y_true=y)

print(f"loss: {loss}")

predictions = np.argmax(activation2.outputs, axis=1)

if len(y.shape) == 2:
    y = np.argmax(y, axis=1)

accuracy = np.mean(predictions == y)
print(f"acc: {accuracy}")