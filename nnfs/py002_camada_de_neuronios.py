# 4 entradas, 3 neuronios


# 1 conjunto (batches) de 4 entradas

entradas = [
    [1,2,3,2.5],
]

# 3 conjuntos de 4 pesos
pesos = [
    [.2, .8, -.5, 1.],
    [.5, -.91, .26, -.5],
    [-.26, -.27, .17, .87],
]


# 1 conjunto de 3 vieses
vieses = [
    [2., 3., .5],
]


# 3 saidas
saidas = [
    # neuronio1
    entradas[0][0]*pesos[0][0] + entradas[0][1]*pesos[0][1] + entradas[0][2]*pesos[0][2] + entradas[0][3]*pesos[0][3] + vieses[0][0],
    # neuronio2
    entradas[0][0]*pesos[1][0] + entradas[0][1]*pesos[1][1] + entradas[0][2]*pesos[1][2] + entradas[0][3]*pesos[1][3] + vieses[0][1],
    # neuronio3
    entradas[0][0]*pesos[2][0] + entradas[0][1]*pesos[2][1] + entradas[0][2]*pesos[2][2] + entradas[0][3]*pesos[2][3] + vieses[0][2],
]

print(f"saidas:\n{saidas}\n")
