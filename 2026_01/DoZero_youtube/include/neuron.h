#ifndef NEURON_H
#define NEURON_H
#include <stdlib.h>
#include <stdint.h>

typedef struct Neuron Neuron;

struct Neuron {
    float *weights;
    uint32_t nconnections;
    float bias;
    float (*actfunc)(float);
    Neuron *conneurons;
};

float computeout(Neuron neuron, float *inputs);

#endif // NEURON_H
