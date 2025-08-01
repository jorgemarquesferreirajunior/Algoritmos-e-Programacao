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
    def __init__(self, qtd_entradas: int = 1, qtd_neuronios: int = 1, verbose=False) -> None:
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
        # self.momento_pesos = np.zeros_like(self.matriz_pesos)
        # self.momento_vieses = np.zeros_like(self.matriz_vieses)

        if verbose:
            self.show_init()

    def show_init(self) -> None:
        print("\nCamada criada com sucesso!")
        print("Pesos inicializados:")
        print(self.matriz_pesos, "\n")
        print("Vieses inicializados:")
        print(self.matriz_vieses, "\n")

    # Propagação direta (forward pass)
    def propagacao(self, entradas: np.ndarray, verbose=False) -> None:
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
    def retropropagacao(self, dvalores: np.ndarray, verbose=False) -> None:
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


# Classe de ativação ReLU
class FaRelu:
    def propagacao(self, entradas: np.ndarray, verbose=False) -> None:
        """
        Aplica a função de ativação ReLU.

        Parâmetros:
            entradas: valores recebidos da camada anterior
        """
        self.matriz_entradas = entradas.copy()
        self.matriz_saidas = np.maximum(0, self.matriz_entradas)

        if verbose:
            print(f"Propagação ReLU:\n{self.matriz_saidas}")

    def retropropagacao(self, dvalores: np.ndarray, verbose=False) -> None:
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
        exponencial_valores = np.exp(self.matriz_entradas - maiores_valores)
        
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



# Classes de perda
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


# classes de otimizacao
class otimizadorSGD:
    def __init__(self, taxa_aprendizagem : float = 1.0, decaimento : float = 0.0, momento : float = 0.0) -> None:
        # Taxa de aprendizado inicial
        self.taxa_aprendizagem = taxa_aprendizagem
        # Taxa de aprendizado ajustada com decaimento (sera atualizada a cada iteracao)
        self.taxa_aprendizagem_atual = taxa_aprendizagem
        # Fator de decaimento (reduz gradualmente a taxa de aprendizado)
        self.decaimento = decaimento
        # Contador de iteracoes (usado para o calculo do decaimento)
        self.iteracoes = 0
        # Fator de momento (ajuda a suavizar as atualizacoes e acelerar o treinamento)
        self.momento = momento


    def prepara_parametros(self):
         # Ajusta a taxa de aprendizado com base no decaimento e no numero de iteracoes
        if self.decaimento:
            self.taxa_aprendizagem_atual = self.taxa_aprendizagem * (1 / (1 + self.decaimento * self.iteracoes))

    def atualiza_parametros(self, camada : Camada_Densa):
        # Se o momento esta ativado, aplica a atualizacao com base no vetor de momento
        if self.momento:
            # Inicializa os vetores de momento se nao existirem ainda na camada
            if not hasattr(camada, 'momento_pesos'):
                camada.momento_pesos = np.zeros_like(camada.matriz_pesos)
                camada.momento_vieses = np.zeros_like(camada.matriz_vieses)

            # Calcula a atualizacao dos pesos com momento
            pesos_atualizados = self.momento * camada.momento_pesos - self.taxa_aprendizagem_atual * camada.matriz_dpesos
            camada.momento_pesos = pesos_atualizados

            # Calcula a atualizacao dos vieses com momento
            vieses_atualizados = self.momento * camada.momento_vieses - self.taxa_aprendizagem_atual * camada.matriz_dvieses
            camada.momento_vieses = vieses_atualizados
        else:
            # Atualizacao padrao do SGD sem uso de momento
            pesos_atualizados = - self.taxa_aprendizagem_atual * camada.matriz_dpesos
            vieses_atualizados = - self.taxa_aprendizagem_atual * camada.matriz_dvieses

        # Aplica as atualizacoes aos pesos e vieses da camada
        camada.matriz_pesos += pesos_atualizados
        camada.matriz_vieses += vieses_atualizados

    def reprepara_parametros(self):
         # Incrementa o contador de iteracoes para ajuste da taxa de aprendizado
        self.iteracoes += 1

class otimizadorAdaGrad:
    def __init__(self, taxa_aprendizagem : float = 1.0, decaimento : float = 0.0, epsilon : float = 1e-7) -> None:
        # Taxa de aprendizado inicial
        self.taxa_aprendizagem = taxa_aprendizagem
        # Taxa de aprendizado ajustada com decaimento (sera atualizada a cada iteracao)
        self.taxa_aprendizagem_atual = taxa_aprendizagem
        # Fator de decaimento (reduz gradualmente a taxa de aprendizado)
        self.decaimento = decaimento
        # Contador de iteracoes (usado para o calculo do decaimento)
        self.iteracoes = 0

        self.epsilon = epsilon


    def prepara_parametros(self):
         # Ajusta a taxa de aprendizado com base no decaimento e no numero de iteracoes
        if self.decaimento:
            self.taxa_aprendizagem_atual = self.taxa_aprendizagem * (1 / (1 + self.decaimento * self.iteracoes))

    def atualiza_parametros(self, camada : Camada_Densa):

        if not hasattr(camada, 'cache_pesos'):
            camada.cache_pesos = np.zeros_like(camada.matriz_pesos)
            camada.cache_vieses = np.zeros_like(camada.matriz_vieses)

        camada.cache_pesos += camada.matriz_dpesos ** 2 
        camada.cache_vieses += camada.matriz_dvieses ** 2 

        # Aplica as atualizacoes aos pesos e vieses da camada
        camada.matriz_pesos += - self.taxa_aprendizagem_atual * camada.matriz_dpesos / (np.sqrt(camada.cache_pesos) + self.epsilon)
        camada.matriz_vieses += - self.taxa_aprendizagem_atual * camada.matriz_dvieses / (np.sqrt(camada.cache_vieses) + self.epsilon)

    def reprepara_parametros(self):
         # Incrementa o contador de iteracoes para ajuste da taxa de aprendizado
        self.iteracoes += 1

class otimizadorRMSProp:
    def __init__(self, taxa_aprendizagem : float = 0.001, decaimento : float = 0.0, epsilon : float = 1e-7, rho : float = 0.9) -> None:
        # Taxa de aprendizado inicial
        self.taxa_aprendizagem = taxa_aprendizagem
        # Taxa de aprendizado ajustada com decaimento (sera atualizada a cada iteracao)
        self.taxa_aprendizagem_atual = taxa_aprendizagem
        # Fator de decaimento (reduz gradualmente a taxa de aprendizado)
        self.decaimento = decaimento
        # Contador de iteracoes (usado para o calculo do decaimento)
        self.iteracoes = 0

        self.epsilon = epsilon

        self.rho = rho


    def prepara_parametros(self):
         # Ajusta a taxa de aprendizado com base no decaimento e no numero de iteracoes
        if self.decaimento:
            self.taxa_aprendizagem_atual = self.taxa_aprendizagem * (1 / (1 + self.decaimento * self.iteracoes))

    def atualiza_parametros(self, camada : Camada_Densa):

        if not hasattr(camada, 'cache_pesos'):
            camada.cache_pesos = np.zeros_like(camada.matriz_pesos)
            camada.cache_vieses = np.zeros_like(camada.matriz_vieses)

        camada.cache_pesos = self.rho * camada.cache_pesos + (1 - self.rho) * camada.matriz_dpesos ** 2
        camada.cache_vieses = self.rho * camada.cache_vieses + (1 - self.rho) * camada.matriz_dvieses ** 2

        # Aplica as atualizacoes aos pesos e vieses da camada
        camada.matriz_pesos += - self.taxa_aprendizagem_atual * camada.matriz_dpesos / (np.sqrt(camada.cache_pesos) + self.epsilon)
        camada.matriz_vieses += - self.taxa_aprendizagem_atual * camada.matriz_dvieses / (np.sqrt(camada.cache_vieses) + self.epsilon)

    def reprepara_parametros(self):
         # Incrementa o contador de iteracoes para ajuste da taxa de aprendizado
        self.iteracoes += 1

class otimizadorAdam:
    def __init__(self, taxa_aprendizagem : float = 0.001, decaimento : float = 0.0, epsilon : float = 1e-7, beta1 : float = 0.9, beta2 : float = 0.999) -> None:
        # Taxa de aprendizado inicial
        self.taxa_aprendizagem = taxa_aprendizagem
        # Taxa de aprendizado ajustada com decaimento (sera atualizada a cada iteracao)
        self.taxa_aprendizagem_atual = taxa_aprendizagem
        # Fator de decaimento (reduz gradualmente a taxa de aprendizado)
        self.decaimento = decaimento
        # Contador de iteracoes (usado para o calculo do decaimento)
        self.iteracoes = 0

        self.epsilon = epsilon

        self.beta_1 = beta1

        self.beta_2 = beta2


    def prepara_parametros(self):
         # Ajusta a taxa de aprendizado com base no decaimento e no numero de iteracoes
        if self.decaimento:
            self.taxa_aprendizagem_atual = self.taxa_aprendizagem * (1 / (1 + self.decaimento * self.iteracoes))

    def atualiza_parametros(self, camada : Camada_Densa):

        if not hasattr(camada, 'cache_pesos'):
            camada.cache_pesos = np.zeros_like(camada.matriz_pesos)
            camada.momento_pesos = np.zeros_like(camada.matriz_pesos)
            camada.cache_vieses = np.zeros_like(camada.matriz_vieses)
            camada.momento_vieses = np.zeros_like(camada.matriz_vieses)

        camada.momento_pesos = self.beta_1 * camada.momento_pesos + (1 - self.beta_1) * camada.matriz_dpesos
        momento_pesos_corretos = camada.momento_pesos / (1 - self.beta_1 ** (self.iteracoes + 1))
        camada.momento_vieses = self.beta_1 * camada.momento_vieses + (1 - self.beta_1) * camada.matriz_dvieses
        momento_vieses_corretos = camada.momento_vieses / (1 - self.beta_1 ** (self.iteracoes + 1))
        
        camada.cache_pesos = self.beta_2 * camada.cache_pesos + (1 - self.beta_2) * camada.matriz_dpesos ** 2
        cache_pesos_corretos = camada.cache_pesos / (1 - self.beta_2 ** (self.iteracoes + 1))
        camada.cache_vieses = self.beta_2 * camada.cache_vieses + (1 - self.beta_2) * camada.matriz_dvieses ** 2
        cache_vieses_corretos = camada.cache_vieses / (1 - self.beta_2 ** (self.iteracoes + 1))

        # Aplica as atualizacoes aos pesos e vieses da camada
        camada.matriz_pesos += - self.taxa_aprendizagem_atual * momento_pesos_corretos / (np.sqrt(cache_pesos_corretos) + self.epsilon)
        camada.matriz_vieses += - self.taxa_aprendizagem_atual * momento_vieses_corretos / (np.sqrt(cache_vieses_corretos) + self.epsilon)

    def reprepara_parametros(self):
         # Incrementa o contador de iteracoes para ajuste da taxa de aprendizado
        self.iteracoes += 1