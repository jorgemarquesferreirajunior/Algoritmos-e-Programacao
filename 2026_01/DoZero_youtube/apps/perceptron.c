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
    uint32_t nsample = 5;
    uint32_t qty_iputs = 2;
    Neuron neuron = initneuron(ident, qty_iputs);
    float ** inputs = mallocmatrix(nsample, qty_iputs);
    float *out_true = (float *)malloc(sizeof(float) * nsample);
    // 4 amostras com 1 uma entrada cada
    inputs[0][0] = 0;inputs[0][1] = 0;
    inputs[1][0] = 2;inputs[1][1] = 15;
    inputs[2][0] = 8;inputs[2][1] = 3;
    inputs[3][0] = 14;inputs[3][1] = 18;
    inputs[4][0] = 20;inputs[4][1] = 1;

    out_true[0] = 5;
    out_true[1] = 41;
    out_true[2] = 35;
    out_true[3] = 83;
    out_true[4] = 67;

    // neuron.weights[0] = 2.5;
    // neuron.bias = 6.0;

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
        train(&neuron, mse, inputs, out_true, nsample, 0.001f);
        // if (2.49<= neuron.weights[0] && neuron.weights[0] <= 2.51) {
        //     if (5.99 <= neuron.bias &&  neuron.bias <= 6.01) {
        //         printf("Epoca atual:  %d\n", epoch);
        //         break;
        //     }
        // }
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
