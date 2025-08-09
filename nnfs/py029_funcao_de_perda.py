# Caso 1 : classes alvo no formato de labels
import numpy as np

class_targets = [0,1,1]

softmax_outputs = np.array([
    [.7, .1, .2],
    [.1, .5, .4],
    [.02, .9, .8]])

# Valores fixados com limites de 1 - 1e-7 ate 1e-7, evitando logaritmo de 0 e 1
softmax_outputs = np.clip(softmax_outputs, 1e-7, 1 - 1e-7)

a = softmax_outputs[range(len(class_targets)), class_targets]

print(f"Predicoes:\n{a}\n")

b = -np.log(a)

print(f"Logs:\n{b}\n")

loss = np.mean(b)

print(f"Perda:\n{loss}\n")

