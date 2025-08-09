from mynnfs import *
from nnfs.datasets import spiral_data

# entradas
X,y = spiral_data(samples=100, classes=3)

# camadas
dense1 = DenseLayer(qtd_inputs=2, qtd_neurons=3)
dense2 = DenseLayer(qtd_inputs=3, qtd_neurons=3)

# funcoes de ativacao
activation1 = ActivationRelu()
activation2 = ActivationSoftmax()

# funcao de perda
loss_function = Loss_CategoricalCrossEntropy()

# forward pass

dense1.forward(inputs=X)
activation1.forward(inputs=dense1.outputs)
dense2.forward(inputs=activation1.outputs)
activation2.forward(inputs=dense2.outputs)
confidences = activation2.output
loss = loss_function.forward(y_pred=confidences, y_true=y)

predictions = np.argmax(confidences, axis=1)
if len(y.shape) == 2:
    y = np.argmax(y, axis=1)
accuracy = np.mean(predictions == y)

print(f"confidences[:5]\n{confidences[:5]}\n")

print(f"predictions[:5]\n{predictions[:5]}\n")

print(f"y[:5]\n{y[:5]}\n")

print(f"loss: {loss}\n")

print(f"acc: {accuracy}\n")
