from mynnfs import *

softmax_outputs = np.array([
    [.7, .1, .2],
    [.1, .5, .4],
    [.02, .9, .08],
])

class_targets = np.array([0, 1, 1])

softmax_loss = ActivationSoftmax_Loss_CategoricalCrossEntropy()
softmax_loss.backward(dvalues=softmax_outputs, y_true=class_targets)

dvalues = softmax_loss.dinputs

print(f"Gradientes: \n{dvalues}\n")