# Caso 2 : classes alvo no formato one-hot encoded
import numpy as np

class_targets = np.array([
    [1, 0, 0],
    [0, 1, 0],
    [0, 1, 0],
])

softmax_outputs = np.array([
    [.7, .1, .2],
    [.1, .5, .4],
    [.02, .9, .8]])

# Valores fixados com limites de 1 - 1e-7 ate 1e-7, evitando logaritmo de 0 e 1
softmax_outputs = np.clip(softmax_outputs, 1e-7, 1 - 1e-7)

a = softmax_outputs * class_targets

print(f"Predicoes:\n{a}\n")

a = np.sum(a, axis=1)

print(f"Predicoes:\n{a}\n")

b = -np.log(a)

print(f"Logs:\n{b}\n")

loss = np.mean(b)

print(f"Perda:\n{loss}\n")

