Aqui est� um programa em C que permite cadastrar alunos, calcular suas m�dias finais e pesquisar por matr�culas:

```c
#include <stdio.h>
#include <string.h>

// Estrutura para representar um aluno
struct Aluno {
    char nome[50];
    int matricula;
    float nota1;
    float nota2;
    float notaRecuperacao; // Nota de recupera��o, caso seja necess�ria
    float mediaFinal;
};

int main() {
    struct Aluno turma[30]; // Array para armazenar os alunos
    int numAlunos = 0; // Contador de alunos cadastrados

    // Menu principal
    int opcao;
    do {
        printf("\nMenu:\n");
        printf("1. Cadastrar aluno\n");
        printf("2. Calcular m�dia final\n");
        printf("3. Pesquisar aluno por matr�cula\n");
        printf("4. Sair\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                if (numAlunos < 30) {
                    // Cadastrar um novo aluno
                    struct Aluno novoAluno;
                    printf("Nome do aluno: ");
                    scanf("%s", novoAluno.nome);
                    printf("Matr�cula do aluno: ");
                    scanf("%d", &novoAluno.matricula);
                    printf("Nota 1 do aluno: ");
                    scanf("%f", &novoAluno.nota1);
                    printf("Nota 2 do aluno: ");
                    scanf("%f", &novoAluno.nota2);
                    printf("Nota de recupera��o do aluno (0 se n�o houver): ");
                    scanf("%f", &novoAluno.notaRecuperacao);

                    // Calcular a m�dia final
                    if (novoAluno.notaRecuperacao == 0) {
                        novoAluno.mediaFinal = (novoAluno.nota1 + novoAluno.nota2) / 2.0;
                    } else {
                        novoAluno.mediaFinal = (novoAluno.nota1 + novoAluno.nota2 + novoAluno.notaRecuperacao) / 3.0;
                    }

                    // Adicionar o aluno � turma
                    turma[numAlunos] = novoAluno;
                    numAlunos++;

                    printf("Aluno cadastrado com sucesso!\n");
                } else {
                    printf("A turma est� cheia (limite de 30 alunos).\n");
                }
                break;

            case 2:
                // Calcular a m�dia final de um aluno
                if (numAlunos > 0) {
                    int matricula;
                    printf("Digite a matr�cula do aluno: ");
                    scanf("%d", &matricula);

                    int encontrado = 0;
                    for (int i = 0; i < numAlunos; i++) {
                        if (turma[i].matricula == matricula) {
                            printf("M�dia final do aluno %s: %.2f\n", turma[i].nome, turma[i].mediaFinal);
                            encontrado = 1;
                            break;
                        }
                    }
                    if (!encontrado) {
                        printf("Aluno n�o encontrado.\n");
                    }
                } else {
                    printf("Nenhum aluno cadastrado.\n");
                }
                break;

            case 3:
                // Pesquisar aluno por matr�cula
                if (numAlunos > 0) {
                    int matricula;
                    printf("Digite a matr�cula do aluno: ");
                    scanf("%d", &matricula);

                    int encontrado = 0;
                    for (int i = 0; i < numAlunos; i++) {
                        if (turma[i].matricula == matricula) {
                            printf("Aluno encontrado:\n");
                            printf("Nome: %s\n", turma[i].nome);
                            printf("Matr�cula: %d\n", turma[i].matricula);
                            printf("Nota 1: %.2f\n", turma[i].nota1);
                            printf("Nota 2: %.2f\n", turma[i].nota2);
                            printf("Nota de recupera��o: %.2f\n", turma[i].notaRecuperacao);
                            printf("M�dia final: %.2f\n", turma[i].mediaFinal);
                            encontrado = 1;
                            break;
                        }
                    }
                    if (!encontrado) {
                        printf("Aluno n�o encontrado.\n");
                    }
                } else {
                    printf("Nenhum aluno cadastrado.\n");
                }
                break;

            case 4:
                printf("Encerrando o programa. At� mais!\n");
                break;

            default:
                printf("Op��o inv�lida. Tente novamente.\n");
                break;
        }
    } while (opcao != 4);

    return 0;
}
```

Neste programa, voc� pode escolher entre cadastrar alunos, calcular a m�dia final de um aluno ou pesquisar um aluno por matr�cula. O programa permite cadastrar at� 30 alunos na turma e calcula a m�dia final levando em considera��o as notas e, se necess�rio, a nota de recupera��o. Certifique-se de que esteja utilizando um compilador C para testar o c�digo.
