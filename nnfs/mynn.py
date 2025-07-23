"""
Firmware completo para uso de redes neurais em Python

Classes:
    - Camada_Densa: Classe que representa uma camada com entradas, pesos e vieses
    - FA_Relu: Classe que representa a função de ativação ReLU
    - Perda_EntropiaCategoricaCruzada: Função de perda baseada em entropia cruzada
"""

import numpy as np

# Camada totalmente conectada (densa)
class Camada_Densa:
    def __init__(self, qtd_entradas: int = 1, qtd_neuronios: int = 1, verbose=True) -> None:
        """
        Inicializa pesos e vieses da camada com valores aleatórios pequenos.

        Parâmetros:
            qtd_entradas: número de entradas (features) recebidas por neurônio
            qtd_neuronios: número de neurônios nesta camada
        """
        # Pesos inicializados com distribuição normal, escalados por 0.01
        self.matriz_pesos = np.random.randn(qtd_entradas, qtd_neuronios) * 0.01
        # Vieses inicializados com zero
        self.matriz_vieses = np.zeros(shape=(1, qtd_neuronios))

        if verbose:
            self.show_init()

    def show_init(self) -> None:
        print("\nCamada criada com sucesso!")
        print("Pesos inicializados:")
        print(self.matriz_pesos, "\n")
        print("Vieses inicializados:")
        print(self.matriz_vieses, "\n")

    # Propagação direta (forward pass)
    def propagacao(self, entradas: np.ndarray, verbose=True) -> None:
        """
        Executa a propagação direta da camada.

        Parâmetros:
            entradas: matriz (batch) de dados de entrada com shape (n_amostras, qtd_entradas)
        """
        # Armazena as entradas para uso posterior na retropropagação
        self.matriz_entradas = entradas.copy()
        # Calcula a saída da camada: produto escalar das entradas com os pesos + vieses
        self.matriz_saidas = np.dot(self.matriz_entradas, self.matriz_pesos) + self.matriz_vieses

        if verbose:
            print(f"Propagação:\n{self.matriz_saidas}\n")

    # Retropropagação (backward pass)
    def retropropagacao(self, dvalores: np.ndarray, verbose=True) -> None:
        """
        Executa a retropropagação da camada densa, calculando os gradientes.

        Parâmetros:
            dvalores: gradiente vindo da próxima camada
        """
        self.matriz_dentradas = np.dot(dvalores, self.matriz_pesos.T)
        self.matriz_dpesos = np.dot(self.matriz_entradas.T, dvalores)
        self.matriz_dvieses = np.sum(dvalores, axis=0, keepdims=True)

        if verbose:
            print("Retropropagação:")
            print("Gradiente das entradas:")
            print(self.matriz_dentradas)
            print("Gradiente dos pesos:")
            print(self.matriz_dpesos)
            print("Gradiente dos vieses:")
            print(self.matriz_dvieses, "\n")


# Função de ativação ReLU
class FaRelu:
    def propagacao(self, entradas: np.ndarray, verbose=True) -> None:
        """
        Aplica a função de ativação ReLU.

        Parâmetros:
            entradas: valores recebidos da camada anterior
        """
        self.matriz_entradas = entradas.copy()
        self.matriz_saidas = np.maximum(0, self.matriz_entradas)

        if verbose:
            print(f"Propagação ReLU:\n{self.matriz_saidas}")

    def retropropagacao(self, dvalores: np.ndarray, verbose=True) -> None:
        """
        Retropropagação da ReLU: calcula gradientes com base nas entradas.

        Parâmetros:
            dvalores: gradiente vindo da próxima camada
        """
        self.matriz_dentradas = dvalores.copy()
        # Zera os gradientes onde a entrada original era menor ou igual a 0
        self.matriz_dentradas[self.matriz_entradas <= 0] = 0

        if verbose:
            print(f"Retropropagação ReLU:\n{self.matriz_dentradas}")

class FaSoftmax():
    def propagacao(self, entradas : np.ndarray) -> None:
        self.matriz_entradas = entradas.copy()

        # Encontra o maior valor em cada linha para evitar problemas numericos
        maiores_valores = np.max(entradas, axis=1, keepdims=True)
        
        # Calcula o exponencial dos valores ajustados
        exponencial_valores = np.exp(entradas - maiores_valores)
        
        # Normaliza os valores exponenciais para que somem 1 (probabilidades)
        self.matriz_saidas = exponencial_valores / np.sum(exponencial_valores, axis=1, keepdims=True)

    def retropropagacao(self, dvalores : np.ndarray) -> None:
        # Cria um array sem inicializacao com o mesmo formato de dvalores
        self.matriz_dentradas = np.empty_like(dvalores)
        
        # Para cada amostra, processa a saida e o gradiente correspondente
        for indice, (saida_individual, derivadas_individuais) in enumerate(zip(self.matriz_saidas, dvalores)):
            
            # Converte a saida para uma matriz coluna
            saida_individual = saida_individual.reshape(-1, 1)
            
            # Calcula a matriz Jacobiana da funcao softmax
            matriz_jacobiana = np.diagflat(saida_individual) - np.dot(saida_individual, saida_individual.T)
            
            # Calcula o gradiente da perda em relacao as entradas
            self.matriz_dentradas[indice] = np.dot(matriz_jacobiana,
                                                   derivadas_individuais)



# Funções de perda
class Perda:
    def calcular(self, saidas_previstas: np.ndarray, saidas_reais: np.ndarray) -> np.ndarray:
        """
        Calcula a média dos erros da função de perda.

        Parâmetros:
            saidas_previstas: previsões da rede
            saidas_reais: valores reais (rótulos)

        Retorna:
            Erro médio
        """
        erros_amostras = self.propagacao(saidas_previstas, saidas_reais)
        media_erros = np.mean(erros_amostras) 
        return media_erros


class Perda_EntropiaCategoricaCruzada(Perda):
    def propagacao(self, saidas_previstas: np.ndarray, saidas_reais: np.ndarray) -> np.ndarray:
        """
        Calcula a perda de entropia cruzada para classificação categórica.

        Parâmetros:
            saidas_previstas: probabilidades previstas (softmax)
            saidas_reais: rótulos reais (índices ou one-hot)

        Retorna:
            Perda para cada amostra
        """
        # Evita log(0)
        saidas_previstas = np.clip(saidas_previstas, 1e-7, 1 - 1e-7)
        qtd_amostras = len(saidas_previstas)
        confiancas_corretas = np.zeros(shape=(saidas_previstas.shape))

        if len(saidas_reais.shape) == 1:
            # Rótulos como índices
            confiancas_corretas = saidas_previstas[range(qtd_amostras), saidas_reais]
        elif len(saidas_reais.shape) == 2:
            # Rótulos como one-hot
            confiancas_corretas = np.sum(saidas_previstas * saidas_reais, axis=1)
        log_confiancas = -np.log(confiancas_corretas)
        return log_confiancas

    def retropropagacao(self, dvalores: np.ndarray, saidas_reais: np.ndarray) -> None:
        """
        Calcula o gradiente da perda em relação às entradas do softmax.

        Parâmetros:
            dvalores: probabilidades previstas
            saidas_reais: rótulos reais (índices ou one-hot)
        """
        qtd_amostras = len(dvalores)
        qtd_etiquetas = len(dvalores[0])

        # Converte rótulos inteiros para one-hot
        if len(saidas_reais.shape) == 1:
            saidas_reais = np.eye(qtd_etiquetas)[saidas_reais]

        # Derivada da entropia cruzada
        self.matriz_dentradas = -saidas_reais / dvalores
        self.matriz_dentradas /= qtd_amostras

class Perda_FaSoftmax_EntropiaCategoricaCruzada():
    def __init__(self) -> None:
        # Inicializa a funcao de ativacao Softmax
        self.funcao_ativacao = FaSoftmax()
        # Inicializa a funcao de perda de entropia categorica cruzada
        self.perda = Perda_EntropiaCategoricaCruzada()

    def propagacao(self, entradas : np.ndarray, saidas_reais : np.ndarray) -> np.ndarray:
        # Executa a propagacao da funcao Softmax
        self.funcao_ativacao.propagacao(entradas=entradas)
        # Armazena as saidas da Softmax
        self.matriz_saidas = self.funcao_ativacao.matriz_saidas
        # Calcula e retorna a perda usando entropia cruzada
        return self.perda.calcular(saidas_previstas=self.matriz_saidas, saidas_reais=saidas_reais)
    
    def retropropagacao(self,dvalores, saidas_reais) -> None:
        # Numero de amostras no batch
        qtd_amostras = len(dvalores)

        # Se as saidas reais estiverem em formato one-hot, converte para inteiros
        if len(saidas_reais.shape) == 2:
            saidas_reais = np.argmax(saidas_reais, axis=1)

        # Copia os valores de entrada para modificar sem afetar os originais
        self.matriz_dentradas = dvalores.copy()
        # Subtrai 1 da classe correta para cada amostra (gradiente da softmax com entropia cruzada)
        self.matriz_dentradas[range(qtd_amostras), saidas_reais] -= 1
        # Divide pela quantidade de amostras para normalizar o gradiente
        self.matriz_dentradas /= qtd_amostras