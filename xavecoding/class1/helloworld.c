#include <stdio.h> // Inclui a biblioteca padrão para entrada e saída, permitindo o uso de funções como printf.
#include <stdlib.h> // Inclui a biblioteca que fornece funções de utilidade geral, como alocação de memória e mnemonicos como EXIT_SUCCESS.

int main(int argc, char *argv[]) { // Função principal do programa. 'argc' é o
                                   // número de argumentos passados, e 'argv' é
                                   // um array de strings com os argumentos.
  printf("Hello World!\n"); // Imprime a mensagem "Hello World!" no terminal.
  return EXIT_SUCCESS;      // Retorna o valor de EXIT_SUCCESS (geralmente 0),
                       // indicando que o programa foi executado com sucesso.
}
