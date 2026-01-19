#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include <stdint.h>

typedef uint32_t u32;

float train[][2] = { // dataset entradas e saidas
	{0,0},
	{1,2},
	{2,4},
	{3,6},
	{4,8},
};

#define train_count (sizeof(train) / sizeof(train[0]))

float cost(float w, float b) // funcao que calcula o 'custo': somatorio da diferenca entre o valor calculado e o valor esperado
{
	float result = 0.0f;
	for (u32 i = 0; i < train_count; i++)
	{
		float x = train[i][0]; // entrada
		float y = x * w + b; // valor calculado
		float d = y - train[i][1]; // diferenca valor esperado - valor calculado
		result += d*d; // eleva ao quadrado para potencializar o resultado, mantando-o sempre positivo
	}

	result /= train_count;
	return result;
}

int main()
{
	srand(time(0));
	// srand(69);
	float w = rand_float() * 10.0f; // valor aleatorio para um peso 'w'
	float b = rand_float() * 10.0f; // valor aleatorio para um vies 'b'

	float eps = 1e-3; // valor pequeno de incremento para calculo da derivada
	float rate = 1e-3; // valor pequeno para calibracao do peso 'w'

	u32 iterator = 0;
	printf("iteration[%d]    ->\tw: %.5f\tb: %.5f\tcost: %.5f\n",iterator , w, b, cost(w, b));
	//for (u32 iterator = 0; iterator < 5000; iterator++) // loop para atualizacao do peso 'w' conforme calculo da derivada 'dscot'
	while(1)
	{
	    float actualcost = cost(w, b);
		float wcost = (cost(w + eps, b) - actualcost) / eps; // calculo da derivada 'wcost' em relacao ao peso 'w'
		float bcost = (cost(w, b + eps) - actualcost) / eps; // calculo da derivada 'bcost' em relacao ao vies 'b'
		w -= wcost * rate; // atualizacao do peso 'w'
		b -= bcost * rate;
		iterator++;
		// printf("iteration[%d] -> w: %.5f\tb: %.5f\tcost: %.5f\n",iterator , w, b, cost(w, b));
		if (cost(w, b) <= 1e-6 || iterator >= 10e3) break;
	}
	printf("iteration[%d] ->\tw: %.5f\tb: %.5f\tcost: %.5f\n",iterator , w, b, cost(w, b));


	return EXIT_SUCCESS;
}
