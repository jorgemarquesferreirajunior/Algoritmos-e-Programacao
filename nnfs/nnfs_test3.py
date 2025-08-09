from mynnfs import *

class_targets2 = np.array([
    [1, 0, 0],
    [0, 1, 0],
    [0, 1, 0],
])

class_targets1 = np.array([0,1,1])

softmax_outputs = np.array([
    [0., .1, .2],
    [.1, .5, .4],
    [.02, .9, .8]])

loss_function = Loss_CategoricalCrossEntropy()

print(f"softmax_outputs:\n{softmax_outputs}\n")

print(f"class_targets1:\n{class_targets1}\n")

print(f"loss:\n{loss_function.calculate(softmax_outputs, class_targets1)}\n")

print(f"class_targets2:\n{class_targets2}\n")

print(f"loss:\n{loss_function.calculate(softmax_outputs, class_targets2)}\n")




