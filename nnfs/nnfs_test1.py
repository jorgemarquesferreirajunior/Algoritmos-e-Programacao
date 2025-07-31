from mynnfs import *

# entradas

inputs = np.array([
    [1., 2., 3., 2.5],
    [.2, 1.7, 1.2, .8],
])

# camadas
dense1 = DenseLayer(qtd_inputs=4, qtd_neurons=6)
dense2 = DenseLayer(qtd_inputs=6, qtd_neurons=3)

# propagacao

dense1.forward(inputs=inputs)
dense2.forward(inputs=dense1.outputs)

print(f"Inputs{inputs.shape}:\n{inputs}\n")

print(f"dense1.shape{dense1.shape}")

print(f"dense1.outputs{dense1.outputs.shape}:\n{dense1.outputs}\n")

print(f"dense2.shape{dense2.shape}")

print(f"dense2.outputs{dense2.outputs.shape}:\n{dense2.outputs}\n")

