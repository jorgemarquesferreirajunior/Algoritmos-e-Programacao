from py012_classe_camada import *
from py015_funcoes_de_ativacao import *
from py019_classe_categorical_cross_entropy import *
import nnfs
import nnfs.datasets
import numpy as np

nnfs.init()

X,y = nnfs.datasets.spiral_data(samples=100, classes=3)

dense1 = Dense_Layer(qtd_inputs=2, qtd_neurons=3)
activation1 = Activation_ReLU()
dense2 = Dense_Layer(qtd_inputs=3, qtd_neurons=3)
activation2 = Activation_Softmax()
loss_function = Loss_CategoricalCrossEntropy()

best_loss = 999999.
best_dense1_weights = dense1.weights.copy()
best_dense1_biases = dense1.biases.copy()
best_dense2_weights = dense2.weights.copy()
best_dense2_biases = dense2.biases.copy()

for i in range(100001):
    
    dense1.weights = 0.05 * np.random.randn(2, 3)
    dense1.biases = 0.05 * np.random.randn(1, 3)
    dense2.weights = 0.05 * np.random.randn(3, 3)
    dense2.biases = 0.05 * np.random.randn(1, 3)

    dense1.forward(inputs=X)
    activation1.forward(inputs=dense1.outputs)
    dense2.forward(inputs=dense1.outputs)
    activation2.forward(inputs=dense2.outputs)


    loss = loss_function.calculate(y_pred=activation2.outputs, y_true=y)
    predictions = np.argmax(activation2.outputs, axis=1)

    if len(y.shape) == 2:
        y = np.argmax(y, axis=1)

    accuracy = np.mean(predictions == y)

    if loss < best_loss:
        best_loss = loss
        best_dense1_weights = dense1.weights.copy()
        best_dense1_biases = dense1.biases.copy()
        best_dense2_weights = dense2.weights.copy()
        best_dense2_biases = dense2.biases.copy()
        print(f"iteracao: {i}, acc: {accuracy}, loss: {loss}")