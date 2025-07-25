import numpy as np

matrizA = [1,2,3]
matrizB = [
    [4,7,10],
    [5,8,11],
    [6,9,12],
]


print(f"\n{'-='*10}\n")
print(f"matrizA:\n{matrizA}\n")
print(f"matrizB:\n{matrizB}\n")

resultado = np.dot(a=matrizA, b=matrizB)
print(f"np.dot(a=matrizA, b=matrizB) = {resultado}")

print(f"\n{'-='*10}\n")
