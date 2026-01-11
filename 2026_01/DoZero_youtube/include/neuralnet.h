#ifndef NEURALNET_H
#define NEURALNET_H
#include "neuron.h"
#include <stdio.h>

float computecost(Neuron neuron, float **inputs, float *out_true, float (*costfunc)(float *, float *, uint32_t), uint32_t samplesize);
float computegradient(Neuron *neuron, float(*costfunc)(float *, float *, uint32_t), float **inputs, float *out_true, float *param, uint32_t samplesize);
void train(Neuron *neuron, float(*costfunc)(float *, float *, uint32_t), float **inputs, float *out_true, uint32_t samplesize, float lr);
void showneuron(const Neuron *neuron);

#endif // NEURALNET_H
