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

    @property
    def shape(self) -> tuple:
        return self._layer_shape
    