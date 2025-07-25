# 4 entradas, 3 neuronios


# 1 conjunto (batches) de 4 entradas

entradas = [1,2,3,2.5]

# 3 conjuntos de 4 pesos
pesos = [
    [.2, .8, -.5, 1.],
    [.5, -.91, .26, -.5],
    [-.26, -.27, .17, .87],
]


# 1 conjunto de 3 vieses
vieses = [2., 3., .5]

saidas = []
for lpeso, vies in zip(pesos, vieses):
    total = 0
    print()
    for peso, entrada in zip(lpeso, entradas):
        total += peso * entrada
        print(f"{entrada = :<10}, {peso = :<10}")
    total += vies
    saidas.append(total)
    print(f"{total = }")
print()

print(f"saidas:\n{saidas}\n")
