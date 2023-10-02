#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include<locale.h>

typedef struct{
    int matricula;
    float nota1, nota2, notaRecuperacao, mediaFinal;
    char nome[50];
}alunos;

void reset();
int main() {
    setlocale(LC_ALL, "Portuguese");
    alunos turma[30];
    int opcao,exit, contAlunos = 0;

    do{
        printf("\nMenu:\n");
        printf("1. Cadastrar aluno\n");
        printf("2. Calcular m�dia final\n");
        printf("3. Pesquisar aluno por matr�cula\n");
        printf("4. Sair\n");
        printf("Escolha uma op��o: ");
        scanf("%d", &opcao);reset();

        switch(opcao){
            case(1)://cadastrar aluno
                if(contAlunos < 30){
                    alunos novoAluno;
                    printf("Nome do aluno: ");
                    scanf("^[\n]", novoAluno.nome);reset();
                    printf("Matr�cula do aluno: ");
                    scanf("%d", &novoAluno.matricula);reset();
                    printf("Nota 1 do aluno: ");
                    scanf("%f", &novoAluno.nota1);reset();
                    printf("Nota 2 do aluno: ");
                    scanf("%f", &novoAluno.nota2);reset();
                    printf("Nota de recupera��o do aluno (0 se n�o houver): ");
                    scanf("%f", &novoAluno.notaRecuperacao);reset();
                    if (novoAluno.notaRecuperacao == 0) {
                        novoAluno.mediaFinal = (novoAluno.nota1 + novoAluno.nota2) / 2.0;
                    } else {
                        novoAluno.mediaFinal = (novoAluno.nota1 + novoAluno.nota2 + novoAluno.notaRecuperacao) / 3.0;
                    }
                    turma[contAlunos] = novoAluno;
                    contAlunos++;
                    printf("Aluno cadastrado com sucesso!\n");
                } else {
                    printf("A turma est� cheia (limite de 30 alunos).\n");
                }
                break;
            case 2://calcular a  media final
                if (contAlunos > 0) {
                    int matricula;
                    printf("Digite a matr�cula do aluno: ");
                    scanf("%d", &matricula);

                    int encontrado = 0;
                    for (int i = 0; i < contAlunos; i++) {
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

            case 3://pesquisar aluno pr matr�cula
                if (contAlunos > 0) {
                    int matricula;
                    printf("Digite a matr�cula do aluno: ");
                    scanf("%d", &matricula);

                    int encontrado = 0;
                    for (int i = 0; i < contAlunos; i++) {
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
        printf("\nSair do programa? ");
        scanf("%d", exit); reset();
    }while(exit != 0);

    return 0;
}

void reset(){
    fflush(stdin);
}


