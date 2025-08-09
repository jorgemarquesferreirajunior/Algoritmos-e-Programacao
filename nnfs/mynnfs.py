""""
Biblioteca para Implementacao de uma rede neural completa do absoluto ZERO 
"""

import numpy as np

class DenseLayer:
    def __init__(self, qtd_inputs : int = 1, qtd_neurons : int = 1) -> None:
        self._layer_shape = (qtd_inputs, qtd_neurons)
        self.weights = np.random.randn(qtd_neurons, qtd_inputs) * .01
        self.biases = np.zeros((1, qtd_neurons))

    def forward(self, inputs : np.ndarray) -> None:
        self.inputs = inputs.copy()
        self.outputs = np.dot(self.inputs, self.weights.T)

    def backward(self, dvalues : np.ndarray) -> None:
        self.dweights = np.dot(self.inputs.T, dvalues)
        self.dbiases = np.sum(dvalues, axis=0, keepdims=True)
        self.dinputs = np.dot(dvalues, self.weights)

    @property
    def shape(self) -> tuple:
        return self._layer_shape
    

class Loss_CategoricalCrossEntropy:
    def forward(self, y_pred : np.ndarray, y_true: np.ndarray) -> np.float64:

        # Limitando valores entre 1 - 1e-7 e 1e-7
        y_clipped = np.clip(y_pred, 1e-7, 1-  1e-7)

        # Caso 1 : classes alvo no formato de labels
        if len(y_true.shape) == 1:
            y_confs = y_clipped[range(len(y_true)), y_true]

        # Caso 2 : classes alvo no formato de one-hot encoded
        else:
            y_confs = np.sum(y_true * y_clipped, axis=1)
        
        y_logs = -np.log(y_confs)

        return np.mean(y_logs)
    
    def backward(self, dvalues : np.ndarray, y_true : np.ndarray) -> None:
        qtd_batches = len(dvalues)
        qtd_labels = len(dvalues[0])

        # Caso 1 : classes alvo no formato de labels
        if len(y_true.shape) == 1:
            y_true = np.eye(qtd_labels)[y_true]
        
        self.dinputs = - y_true / dvalues
        self.dinputs /= qtd_batches


class ActivationRelu:
    def forward(self, inputs : np.ndarray) -> None:
        self.inputs = inputs
        self.outputs = np.maximum(0, inputs)

    def backward(self, dvalues : np.ndarray) -> None:
        self.dinputs = dvalues.copy()
        self.dinputs[self.inputs <= 0] = 0

class ActivationSoftmax:
    def forward(self, inputs : np.ndarray) -> None:
        self.inputs = inputs

        max_inputs = np.max(inputs, axis=1, keepdims=True)
        norm_inputs = inputs - max_inputs
        exp_inputs = np.exp(norm_inputs)
        sum_inputs = np.sum(exp_inputs, axis=1, keepdims=True)

        self.output = exp_inputs / sum_inputs

class ActivationSoftmax_Loss_CategoricalCrossEntropy:
    def __init__(self) -> None:
        self.activation = ActivationSoftmax()
        self.loss = Loss_CategoricalCrossEntropy()

    def forward(self, inputs : np.ndarray, y_true : np.ndarray) -> np.float64:
        self.activation.forward(inputs)
        self.outputs = self.activation.output

        return self.loss.forward(self.outputs, y_true)
    
    def backward(self, dvalues : np.ndarray, y_true : np.ndarray) -> None:
        qtd_batches = len(dvalues)

        if len(y_true.shape) == 2:
            y_true = np.argmax(y_true, axis=1)
        
        self.dinputs = dvalues.copy()
        self.dinputs[range(qtd_batches), y_true] -= 1
        self.dinputs /= qtd_batches
