from py019_classe_categorical_cross_entropy import *
import numpy as np

softmax_outputs = np.array([
    [.7, .1, .2],
    [.1, .5, .4],
    [.02, .9, .08]
])

# resultados alvo
class_targets = np.array([0,1,1])

y_true_check = np.array([
    [1,0,0],
    [0,1,0],
    [0,1,0]
])

loss = Loss_CategoricalCrossEntropy()

loss1 = loss.calculate(y_pred=softmax_outputs, y_true=class_targets)
print(loss1)

loss2 = loss.calculate(y_pred=softmax_outputs, y_true=y_true_check)
print(loss2)
