import numpy as np

class Activation_ReLU:
    def forward(self, inputs : np.ndarray ):
        self.inputs = inputs.copy()
        # retorna 1 se entrada > 0, senao retorna 0
        self.outputs = np.maximum(0, self.inputs)

class Activation_Softmax():
    def forward(self, inputs : np.ndarray):
        self.inputs = inputs.copy()
        # valores normalizados - para cada linha de entradas, subtrai o maior valor de cada linha em todos os valores de cada linha
        norm_inputs = self.inputs - np.max(self.inputs, axis=1, keepdims=True)

        # calcula o exponencial de cada um dos valores na matriz
        exp_values = np.exp(norm_inputs)

        # calcula o somatorio de todos os valores exponenciais
        sum_lines = np.sum(exp_values, axis=1, keepdims=True)

        # calcula as probabilidades de cada valor
        probabilities = exp_values / sum_lines

        self.outputs = probabilities.copy()
