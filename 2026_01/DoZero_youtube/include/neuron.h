#ifndef NEURON_H
#define NEURON_H
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    float *weights;
    uint32_t nconnections;
    float bias;
    float (*actfunc)(float);
} Neuron;

float computeout(Neuron neuron, float *inputs);
Neuron initneuron(float (*actfunc)(float), uint32_t nconnections);
#endif // NEURON_H
