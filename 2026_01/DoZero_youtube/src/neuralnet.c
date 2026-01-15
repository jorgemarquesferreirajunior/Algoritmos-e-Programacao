#include "neuralnet.h"
#include "neuron.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>



float computecost(Neuron neuron, float **inputs, float *out_true, float (*costfunc)(float *, float *, uint32_t), uint32_t samplesize){
    float *out_pred = (float *)malloc(sizeof(float) * samplesize);
    for (uint32_t k = 0; k < samplesize; k++) {
        out_pred[k] = computeout(neuron, inputs[k]);
    }
    float cost = costfunc(out_true, out_pred, samplesize);
    free(out_pred);
    return cost;
}

float computegradient(Neuron *neuron, float(*costfunc)(float *, float *, uint32_t), float **inputs, float *out_true, float *param, uint32_t samplesize){

    // float learning_rate = 0.0001;
    float eps = 1e-4;
    float original = *param;

    *param = original + eps;
    float cost_plus = computecost(*neuron, inputs, out_true, costfunc, samplesize);

    *param = original - eps;
    float cost_minus = computecost(*neuron, inputs, out_true, costfunc, samplesize);

    *param = original; // restaura

    return (cost_plus - cost_minus) / (1.0f * eps);

}

void train(Neuron *neuron, float(*costfunc)(float *, float *, uint32_t), float **inputs, float *out_true, uint32_t samplesize, float lr){
    float gradient = 0;
    for (uint32_t w = 0; w < neuron->nconnections; w++) {
        gradient = computegradient(neuron, costfunc, inputs, out_true, &neuron->weights[w], samplesize);
        neuron->weights[w] -= lr * gradient;
    }
    gradient = computegradient(neuron, costfunc, inputs, out_true, &neuron->bias, samplesize);
    neuron->bias -= lr * gradient;
}

void showneuron(const Neuron *neuron){
    printf("-----------------------------------------\n");
    printf("Bias: %f\n", neuron->bias);
    printf("Qtd conexoes: %d\n", neuron->nconnections);
    printf("Pesos: \n");
    for (uint32_t i = 0; i < neuron->nconnections; i++) {
        printf("%f\n", neuron->weights[i]);
    }
    printf("-----------------------------------------\n\n");
}
