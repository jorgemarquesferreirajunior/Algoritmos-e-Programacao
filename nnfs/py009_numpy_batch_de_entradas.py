import numpy as np

# 4 entradas, 3 neuronios


# 1 conjunto (batches) de 4 entradas

entradas = [
    [1,2,3,2.5], # batch 1
    [2,4,6,5], # batch 2
]

# 3 conjuntos de 4 pesos
pesos = [
    [.2, .8, -.5, 1.], # pesos neuronio 1
    [.5, -.91, .26, -.5],# pesos neuronio 2
    [-.26, -.27, .17, .87],# pesos neuronio 3
]


# 1 conjunto de 3 vieses
vieses = [
    [2., 3., .5],
]

saidas = np.dot(a=np.array(entradas), b=np.array(pesos).T) + vieses

print(f"saidas:\n{saidas}\n")
