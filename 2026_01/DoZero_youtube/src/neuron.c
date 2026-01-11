#include "neuron.h"
#include "utils.h"
#include <stdlib.h>
#include <stdint.h>

struct Neuron{
    float *weights;
    uint32_t nconnections;
    float bias;
    float (*actfunc)(float);
};

float computeout(Neuron neuron, float *inputs){
    float sum = 0;
    for (uint32_t k = 0; k < neuron.nconnections; k++) {
        sum += neuron.weights[k] * inputs[k];
    }

    return neuron.actfunc(sum + neuron.bias);
}

Neuron initneuron(float (*actfunc)(float), uint32_t nconnections){
    Neuron neuron;
    neuron.actfunc = actfunc;
    neuron.bias = randomize(-1,1);
    neuron.weights = (float *)malloc(sizeof(float) * nconnections);
    neuron.nconnections = nconnections;
    for (uint32_t i = 0; i < nconnections; i++) {
        neuron.weights[i] = randomize(-5.0, 5.0);
    }

    return neuron;
}
