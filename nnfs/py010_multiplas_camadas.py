import numpy as np

# camada de entrada
# camada 1 -> 4 entradas
entradas = np.array([
    [1., 2., 3., 2.5], # batch 1
    [2., 5., -1., 2.], # batch 2
    [-1.5, 2.7, 3.3, -.8], # batch 3
])

# camadas escondidas
# camada 2 -> 3 neuronios, 4 entradas
pesos1 = np.array([
    [.2, .8, -.5, 1.], # neuronio 1
    [.5, -.91, .26, -.5],# neuronio 2
    [-.26, -.27, .17, .87] # neuronio 3
])

vieses1 = np.array([2., 3., .5])

# camada de saida
# camada 3 -> 3 neuronios, 3 entradas
pesos2 = np.array([
    [.1, -.14, .5],
    [-.5, .12, -.33],
    [-.44, .73, -.13]
])
vieses2 = np.array([-1., 2., -.5])

saida1 = np.dot(a=entradas, b=pesos1.T) + vieses1
saida2 = np.dot(a=saida1, b=pesos2.T) + vieses2

print()
print(saida1)
print()
print(saida2)