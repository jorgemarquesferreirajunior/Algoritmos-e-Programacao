#include <iostream>

using namespace std;

// Passagem por valor: altera apenas a vari�vel interna x
void func(int x)
{
    cout << "x (dentro de func, antes)= " << x << endl;
    x = 4;
    cout << "x (dentro de func, depois)= " << x << endl;
}

// Passagem por ponteiro: altera a vari�vel original usando o endere�o
void func_p(int *x)
{
    cout << "x (dentro de func_p, antes)= " << *x << endl;
    *x = 4;
    cout << "x (dentro de func_p, depois)= " << *x << endl;
}

// Passagem por refer�ncia: altera a vari�vel original diretamente
void func_r(int &x)
{
    cout << "x (dentro de func_r, antes)= " << x << endl;
    x = 4;
    cout << "x (dentro de func_r, depois)= " << x << endl;
}

int main()
{
    int y = 3;
    cout << "y (antes da chamada de funcao)= " << y << endl;

    // Chame uma das fun��es para ver o efeito
    // func(y);      // Passagem por valor
    // func_p(&y);   // Passagem por ponteiro
    func_r(y);     // Passagem por refer�ncia

    cout << "y (depois da chamada de funcao)= " << y << endl;
    return 0;
}
