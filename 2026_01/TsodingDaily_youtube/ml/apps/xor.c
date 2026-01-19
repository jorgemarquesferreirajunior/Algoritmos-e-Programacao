#include <stdio.h>                                                                                                                                               
#include <stdlib.h>                                                                                                                                              
#include "utils.h"                                                                                                                                               
#include <time.h>                                                                                                                                                
#include <stdint.h>                                                                                                                                              
                                                                                                                                                                 
typedef uint32_t u32;
typedef float sample[3];

sample or_train[] = { // dataset entradas e saidas Porta logica OR
	{0,0,0},
	{0,1,1},
	{1,0,1},
	{1,1,1},
};

float and_train[][3] = { // dataset entradas e saidas Porta logica AND
	{0,0,0},
	{0,1,0},
	{1,0,0},
	{1,1,1},
};

float nand_train[][3] = { // dataset entradas e saidas Porta logica NAND
	{0,0,1},
	{0,1,0},
	{1,0,0},
	{1,1,0},
};

float xor_train[][3] = { // dataset entradas e saidas Porta logica XOR
	{0,0,0},
	{0,1,1},
	{1,0,1},
	{1,1,0},
};

typedef struct {
    float or_w1;
    float or_w2;
    float or_b;

    float nand_w1;
    float nand_w2;
    float nand_b;

    float and_w1;
    float and_w2;
    float and_b;

}Xor;

float forward(Xor model, float x1, float x2)
{
    float a, b;
    a           = sigmoidf(x1 * model.or_w1      +       x2 * model.or_w2   +     model.or_b);
    b           = sigmoidf(x1 * model.nand_w1    +       x2 * model.nand_w2 +     model.nand_b);
    return        sigmoidf(a * model.and_w1     +       b * model.and_w2  +     model.and_b); 
}

sample *train = xor_train;
size_t train_count = 4; 

float cost(Xor model) // funcao que calcula o 'custo': somatorio da diferenca entre o valor calculado e o valor esperado
{
	float result = 0.0f;
	for (u32 i = 0; i < train_count; i++)
	{
	    float x1 = train[i][0]; // entradas
		float x2 = train[i][1]; // entradas
		float y = forward(model, x1, x2); // valor calculado
		float d = y - train[i][2]; // diferenca valor esperado - valor calculado
		result += d*d; // eleva ao quadrado para potencializar o resultado, mantando-o sempre positivo
	}

	result /= train_count;
	return result;
}

Xor initXor(void)
{
    Xor model;
    model.or_w1 = rand_float();
    model.or_w2 = rand_float();
    model.or_b = rand_float();
    model.nand_w1 = rand_float();
    model.nand_w2 = rand_float();
    model.nand_b = rand_float();
    model.and_w1 = rand_float();
    model.and_w2 = rand_float();
    model.and_b = rand_float();

    return model;
}

void printXor(const Xor *model)
{
    printf("or_w1   = %f\n", model->or_w1);
    printf("or_w2   = %f\n", model->or_w2);
    printf("or_b    = %f\n", model->or_b);
    printf("nand_w1 = %f\n", model->nand_w1);
    printf("nand_w2 = %f\n", model->nand_w2);
    printf("nand_b  = %f\n", model->nand_b);
    printf("and_w1  = %f\n", model->and_w1);
    printf("and_w2  = %f\n", model->and_w2);
    printf("and_b   = %f\n", model->and_b);
}

Xor finite_difference(Xor model, float eps)
{
    Xor g;
    float c = cost(model);

    float aux = model.or_w1;
    model.or_w1 += eps;
    g.or_w1 = ((cost(model) - c) / eps);
    model.or_w1 = aux;

    aux = model.or_w2;
	model.or_w2 += eps;
	g.or_w2 = ((cost(model) - c) / eps);
	model.or_w2 = aux;
    
    aux = model.or_b;
	model.or_b += eps;
	g.or_b = ((cost(model) - c) / eps);
	model.or_b = aux;
    
    aux = model.nand_w1;
	model.nand_w1 += eps;
	g.nand_w1 = ((cost(model) - c) / eps);
	model.nand_w1 = aux;
    
    aux = model.nand_w2;
	model.nand_w2 += eps;
	g.nand_w2 = ((cost(model) - c) / eps);
	model.nand_w2 = aux;
    
    aux = model.nand_b;
	model.nand_b += eps;
	g.nand_b = ((cost(model) - c) / eps);
	model.nand_b = aux;
    
    aux = model.and_w1;
	model.and_w1 += eps;
	g.and_w1 = ((cost(model) - c) / eps);
	model.and_w1 = aux;
    
    aux = model.and_w2;
	model.and_w2 += eps;
	g.and_w2 = ((cost(model) - c) / eps);
	model.and_w2 = aux;
    
    aux = model.and_b;
	model.and_b += eps;
	g.and_b = ((cost(model) - c) / eps);
	model.and_b = aux;

    return g;

}

Xor learn(Xor model, Xor gradient, float rate)
{

    model.or_w1 -= rate * gradient.or_w1;
    model.or_w2 -= rate * gradient.or_w2;
    model.or_b -= rate * gradient.or_b;
    model.nand_w1 -= rate * gradient.nand_w1;
    model.nand_w2 -= rate * gradient.nand_w2;
    model.nand_b -= rate * gradient.nand_b;
    model.and_w1 -= rate * gradient.and_w1;
    model.and_w2 -= rate * gradient.and_w2;
    model.and_b -= rate * gradient.and_b;

    return model;
}
int main(void)
{
    Xor m = initXor(); 
    
    float eps = 1e-1f, rate = 1e-1f;
    
    for (size_t i = 0; i < 1e6; i++) {
        Xor g = finite_difference(m, eps);
        m = learn(m, g, rate);
        // printf("cost    = %f\n", cost(m));
    }

    printf("------------------------------------------\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("%.0f | %.0f = %f\n", train[i][0], train[i][1], forward(m, train[i][0], train[i][1]));
    }
    
    return EXIT_SUCCESS;
}



