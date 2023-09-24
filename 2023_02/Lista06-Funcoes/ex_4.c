#include <stdio.h>
#include <ctype.h>
#include<locale.h>

int main() {
    setlocale(LC_ALL, "Portuguese");
    char ch;

    printf("Digite um caractere qualquer [ex:j]: ");
    ch = getchar();

    if (isalpha(ch)) {
        printf("%c � uma letra.\n", ch);
    } else {
        printf("%c N�o � uma letra.\n", ch);
    }

    if (isdigit(ch)) {
        printf("%c � um d�gito.\n", ch);
    } else {
        printf("%c N�o � um d�gito.\n", ch);
    }

    if (islower(ch)) {
        printf("%c Est� em min�sculas.\n", ch);
    } else {
        printf("%c N�o est� em min�sculas.\n", ch);
    }

    if (isupper(ch)) {
        printf("%c Est� em mai�sculas.\n", ch);
    } else {
        printf("%c N�o est� em mai�sculas.\n", ch);
    }

    if (isspace(ch)) {
        printf("%c � um espa�o em branco.\n", ch);
    } else {
        printf("%c N�o � um espa�o em branco.\n", ch);
    }

    return 0;
}
