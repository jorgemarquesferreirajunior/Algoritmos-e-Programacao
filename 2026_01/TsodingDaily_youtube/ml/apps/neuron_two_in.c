#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include <stdint.h>

typedef uint32_t u32;

float train[][3] = { // dataset entradas e saidas Prta logica OR
	{0,0,0},
	{0,1,1},
	{1,0,1},
	{1,1,0},
};

#define train_count (sizeof(train) / sizeof(train[0]))

float cost(float w1, float w2, float b) // funcao que calcula o 'custo': somatorio da diferenca entre o valor calculado e o valor esperado
{
	float result = 0.0f;
	for (u32 i = 0; i < train_count; i++)
	{
	    float x1 = train[i][0]; // entradas
		float x2 = train[i][1]; // entradas
		float y = sigmoidf(x1 * w1 + x2 * w2 + b); // valor calculado
		float d = y - train[i][2]; // diferenca valor esperado - valor calculado
		result += d*d; // eleva ao quadrado para potencializar o resultado, mantando-o sempre positivo
	}

	result /= train_count;
	return result;
}

int main()
{
    srand(time(0));
	// srand(69);
	float w1 = rand_float(); // valor aleatorio para um peso 'w1'
	float w2 = rand_float(); // valor aleatorio para um peso 'w2'
	float b = rand_float(); // valor aleatorio para um vies 'b'

    float eps = 1e-3; // valor pequeno de incremento para calculo da derivada
	float rate = 1e-1; // valor pequeno para calibracao do peso 'w'
	u32 epochs = 3e4;

	for (u32 iterator = 0; iterator < epochs; iterator++) // loop para atualizacao do peso 'w' conforme calculo da derivada 'dscot'
	{
	    float actualcost = cost(w1, w2, b);
		float dw1 = (cost(w1 + eps, w2, b) - actualcost) / eps; // calculo da derivada 'w1cost' em relacao ao peso 'w1'
		float dw2 = (cost(w1, w2 + eps, b) - actualcost) / eps; // calculo da derivada 'w2cost' em relacao ao peso 'w2'
		float db  = (cost(w1, w2, b + eps) - actualcost) / eps; // calculo da derivada 'bcost' em relacao ao vies 'b'
		w1 -= dw1 * rate; // atualizacao do peso 'w1'
		w2 -= dw2 * rate; // atualizacao do peso 'w2'
		b  -= db  * rate; // atualizacao do vies 'b'
    
        // printf("%f\n", actualcost);
	}

    for (u32 i = 0; i < train_count; i++)
    {
        printf("%.0f | %.0f = %f\t esperado: %f\n", train[i][0], train[i][1], sigmoidf(train[i][0] * w1 + train[i][1] * w2 + b), train[i][2]);
    }

    return EXIT_SUCCESS;
}
