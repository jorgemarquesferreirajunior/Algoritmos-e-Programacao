"""
from nnfs import *

matriz_entradas = np.array([
    [1., 2., 3., 4.],
])
dvalores = np.array([
    [1., 1., 1.],
])


camada1 = Camada_Densa(qtd_entradas=4, qtd_neuronios=3)

camada1.propagacao(entradas=matriz_entradas)

camada1.retropropagacao(dvalores=dvalores)
"""

from mynn import*
from nnfs.datasets import spiral_data

X, y = spiral_data(samples=100, classes=3)

camada1 = Camada_Densa(qtd_entradas=2, qtd_neuronios=3)
funcao1 = FaRelu()

camada2 = Camada_Densa(qtd_entradas=3, qtd_neuronios=3)
funcao2 = Perda_FaSoftmax_EntropiaCategoricaCruzada()


camada1.propagacao(entradas=X, verbose=False)
funcao1.propagacao(entradas=camada1.matriz_saidas, verbose=False)

camada2.propagacao(entradas=funcao1.matriz_saidas, verbose=False)
resultado_perda = funcao2.propagacao(entradas=camada2.matriz_saidas, saidas_reais=y)

print("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=\n")

print(f"Saida funcao de perda:\n{funcao2.matriz_saidas[:5]}")

print(f"Perda: {resultado_perda}\n")

if len(y.shape) == 2:
    y = np.argmax(y, axis=1)

predicoes = np.argmax(funcao2.matriz_saidas, axis=1)
acuracia = np.mean(predicoes == y)

print(f"Acuracia: {acuracia}\n")

funcao2.retropropagacao(dvalores=funcao2.matriz_saidas, saidas_reais=y)
camada2.retropropagacao(dvalores=funcao2.matriz_dentradas, verbose=False)
funcao1.retropropagacao(dvalores=camada2.matriz_dentradas, verbose=False)
camada1.retropropagacao(dvalores=funcao1.matriz_dentradas, verbose=False)

print("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=\n")
print(camada1.matriz_dpesos)
print("")
print(camada1.matriz_dvieses)
print("")
print(camada2.matriz_dpesos)
print("")
print(camada2.matriz_dvieses)




