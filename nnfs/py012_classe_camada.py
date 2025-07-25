import numpy as np

class Dense_Layer():
    def __init__(self, qtd_inputs : int = 1, qtd_neurons : int = 1) -> None:
        """
        Inicializacao dos pesos e vieses

        Parametros
        qtd_neurons: quantidade de neuronios da camada
        qtd_inputs: quantidade de entradas da camada

        """
        # qtd_neurons = qtd de linhas, qtd_inputs = qtd de colunas
        self.weights = 0.01 * np.random.randn(qtd_neurons, qtd_inputs)
        # tranpor a matriz para simplficar o calculo da propagacao
        self.weights = self.weights.T

        self.biases = np.zeros(shape=(1, qtd_neurons))

    # propagacao
    def forward(self, inputs : np.ndarray) -> None:
        self.inputs = inputs.copy()
        self.outputs = np.dot(self.inputs, self.weights) + self.biases