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

"""
## Teste da propagacao e retropropagacao
from mynn import*
from nnfs.datasets import spiral_data

X, y = spiral_data(samples=100, classes=3)

camada1 = Camada_Densa(qtd_entradas=2, qtd_neuronios=3)
funcao1 = FaRelu()

camada2 = Camada_Densa(qtd_entradas=3, qtd_neuronios=3)
funcao2 = Perda_FaSoftmax_EntropiaCategoricaCruzada()


camada1.propagacao(entradas=X)
funcao1.propagacao(entradas=camada1.matriz_saidas)

camada2.propagacao(entradas=funcao1.matriz_saidas)
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
camada2.retropropagacao(dvalores=funcao2.matriz_dentradas)
funcao1.retropropagacao(dvalores=camada2.matriz_dentradas)
camada1.retropropagacao(dvalores=funcao1.matriz_dentradas)

print("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=\n")
print(camada1.matriz_dpesos)
print("")
print(camada1.matriz_dvieses)
print("")
print(camada2.matriz_dpesos)
print("")
print(camada2.matriz_dvieses)
"""

## Teste do otimizador SGD
from mynn import*
from nnfs.datasets import spiral_data

X, y = spiral_data(samples=100, classes=3)

camada1 = Camada_Densa(qtd_entradas=2, qtd_neuronios=64)
funcao1 = FaRelu()

camada2 = Camada_Densa(qtd_entradas=64, qtd_neuronios=3)
funcao2 = Perda_FaSoftmax_EntropiaCategoricaCruzada()

otimizador = otimizadorSGD()


for epoca in range(20001):

    camada1.propagacao(entradas=X)
    funcao1.propagacao(entradas=camada1.matriz_saidas)

    camada2.propagacao(entradas=funcao1.matriz_saidas)
    resultado_perda = funcao2.propagacao(entradas=camada2.matriz_saidas, saidas_reais=y)

    if len(y.shape) == 2:
        y = np.argmax(y, axis=1)

    predicoes = np.argmax(funcao2.matriz_saidas, axis=1)
    acuracia = np.mean(predicoes == y)
    acuracia_ = round(acuracia, 3)

    if not epoca % 100:
        print(f"epoca: {epoca:<10} acuracia: {acuracia_:<10} perda: {resultado_perda:.3f}")

    funcao2.retropropagacao(dvalores=funcao2.matriz_saidas, saidas_reais=y)
    camada2.retropropagacao(dvalores=funcao2.matriz_dentradas)
    funcao1.retropropagacao(dvalores=camada2.matriz_dentradas)
    camada1.retropropagacao(dvalores=funcao1.matriz_dentradas)

    otimizador.atualiza_parametros(camada=camada1)
    otimizador.atualiza_parametros(camada=camada2)



