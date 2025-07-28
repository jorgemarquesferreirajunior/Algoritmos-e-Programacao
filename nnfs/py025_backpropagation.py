import numpy as np

inputs = np.array([1.,2.,3.,4.])
weights = np.array([
    [.1, .2, .3, .4],
    [.5, .6, .7, .8],
    [.9, 1., 1.1, 1.2],
])
biases = np.array([.1, .2, .3])
learning_rate = .001

def relu(x):
    return np.maximum(0, x)

def drelu(x):
    return np.where(x > 0, 1.0, 0.)

loss = 0.

for iteration in range(200):
    # Propagation
    z = np.dot(inputs, weights.T) + biases
    a = relu(z)
    y = np.sum(a)

    loss = y ** 2

    # Backpropagation
    dloss_dy = 2 * y # Gradiente entre a perda e a saida

    dy_da = np.ones_like(a) # Gradiente entre a saida e funcao de ativacao

    dloss_da = dloss_dy * dy_da # Gradiente entre a funcao de ativacao e o somatorio

    da_dz = drelu(z)

    dloss_dz = dloss_da * da_dz # Gradiente entre a perda e o somatorio

    dloss_dw = np.outer(dloss_dz, inputs) # Gradiente entre a perda e os pesos

    dloss_db = dloss_dz # Gradiente entre a perda e o bias

    weights -= learning_rate * dloss_dw
    biases -= learning_rate * dloss_db

    if iteration % 20 == 0: print(f"iteration: {iteration}, loss: {loss}")
    # print(f"iteration: {iteration}, loss: {loss}")

print(f"Final weights:\n{weights}\n")
print(f"Final biases:\n{biases}\n")
print(f"Final loss:\n{loss}\n")





