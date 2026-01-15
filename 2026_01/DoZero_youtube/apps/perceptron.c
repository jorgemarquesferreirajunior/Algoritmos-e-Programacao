#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "neuron.h"
#include "utils.h"
#include "netmath.h"
#include "neuralnet.h"


int main(int argc, char *argv[]){
    srand(time(NULL));

    uint32_t nsample = 6;
    uint32_t qty_iputs = 2;
    
    Neuron neuron = initneuron(sigmoid, qty_iputs);
    
    float ** inputs = mallocmatrix(nsample, qty_iputs);
    float *out_true = (float *)malloc(sizeof(float) * nsample);
    
    inputs[0][0] = 6;inputs[0][1] = 1;
    inputs[1][0] = 5;inputs[1][1] = 0;
    inputs[2][0] = 4;inputs[2][1] = 1;
    inputs[3][0] = 1;inputs[3][1] = 4;
    inputs[4][0] = 1;inputs[4][1] = 2;
    inputs[5][0] = 2;inputs[5][1] = 3;

    out_true[0] = 1;
    out_true[1] = 1;
    out_true[2] = 1;
    out_true[3] = 0;
    out_true[4] = 0;
    out_true[5] = 0;

    neuron.weights[0] = 0.670970f;
    neuron.weights[1] = -0.182195f;
    neuron.bias = -0.891031f;

    float c = computecost(neuron, inputs, out_true, mse, nsample);

    #ifdef ENABLE_PAUSE
        pause_and_clear();
    #endif

    printf("Pesos: ");
    for (uint32_t i = 0; i < qty_iputs; i++) {
        printf("%f ", neuron.weights[i]);
    }

    printf("\n");
    printf("Bias: ");
    printf("%f ", neuron.bias);
    printf("\n");
    printf("Custo antes do treinamento:\t%f\n", c);

    uint32_t epochs = 10000;
    printf("\nTreinamento de %d epocas:\n", epochs);
    for (uint32_t epoch = 0; epoch < epochs; epoch++) {
        train(&neuron, mse, inputs, out_true, nsample, 0.01f);
        
        if (computecost(neuron, inputs, out_true, mse, nsample) <= .0001f) {
            printf("Epoca atual:  %d\n", epoch);
            break;
        }
    }

    c = computecost(neuron, inputs, out_true, mse, nsample);


    printf("Pesos: ");
    for (uint32_t i = 0; i < qty_iputs; i++) {
        printf("%f ", neuron.weights[i]);
    }
    printf("\n");
    printf("Bias: ");
    printf("%f ", neuron.bias);
    printf("\n");
    printf("Custo depois do treinamento:\t%f\n", c);
    for (uint32_t i = 0; i < nsample; i++) {
        float y = computeout(neuron, inputs[i]);
        printf("Entradas %.2f %.2f \t out_true %.2f \t out_pred %.2f\n", inputs[i][0], inputs[i][1], out_true[i], y);
    }
    return EXIT_SUCCESS;
}
