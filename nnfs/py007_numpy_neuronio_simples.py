import numpy as np

entradas = [1,2,3,]
pesos = [0.2,0.8,-0.5,]
vieses = [2.0,]

# saida = entradas[0]*pesos[0] + entradas[1]*pesos[1] + entradas[2]*pesos[2] + vieses[0]
saida = np.dot(a=entradas, b=pesos) + vieses

print(f"saida:\n{saida}\n")
