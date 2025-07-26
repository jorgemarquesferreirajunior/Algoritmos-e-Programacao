from py015_funcoes_de_ativacao import *
from py012_classe_camada import *
import nnfs
import nnfs.datasets
import numpy as np

y_pred = [.7, .1, .2]
y_true = [1,0,0]

loss = - np.log(y_true[0]*y_pred[0] + y_true[1]*y_pred[1] + y_true[2]*y_pred[2])
print(f"{loss = }")
loss = - np.log(y_true[0]*y_pred[0])
print(f"{loss = }")

"""
Para otimizar o calculo, basta fazer -log() do valor predito cuja posicao do array tenha valor 1 no array ds valores de referencia y_true

Por isso foi possivel simplficar o calculo da perda na segunda espressao
"""

softmax_outputs = np.array([
    [.7, .1, .2],
    [.1, .5, .4],
    [.02, .9, .08]
])

# resultados alvo
class_targets = [0,1,1] 

# obtendo apenas os valores preditos correspondentes a referencia
filtered_softmax_outputs = softmax_outputs[range(len(class_targets)), class_targets]
print(filtered_softmax_outputs)

# calculando a perda de cada valor
loss_outputs = - np.log(filtered_softmax_outputs)
print(loss_outputs)

# calculando a media da perda 
mean_loss_outputs = np.mean(loss_outputs)
print(mean_loss_outputs)


# Segunda forma de representar os valores alvo

y_true_check = np.array([
    [1,0,0],
    [0,1,0],
    [0,1,0]
])


y_pred_clipped_check = np.array([
    [.7, .2, .1],
    [.1, .5, .4],
    [.02, .9, .08]
])

a = y_true_check * y_pred_clipped_check
b = np.sum(a, axis=1)
c = -np.log(b)
d = np.mean(c)

print(a)
print(b)
print(c)
print(d)
