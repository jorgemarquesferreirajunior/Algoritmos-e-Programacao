#include "neuron.h"
#include "utils.h"
#include <stdlib.h>
#include <stdint.h>

float computeout(Neuron neuron, float *inputs){
    float sum = 0;
    
    if (neuron.conneurons != NULL){
	    for (uint32_t i = 0; i < neuron.nconnections; i++){
	        sum += computeout(neuron.conneurons[i], inputs) * neuron.weights[i];
	    }
    }
    else {
    	for (uint32_t k = 0; k < neuron.nconnections; k++) {
        	sum += neuron.weights[k] * inputs[k];
    	}
    }
    sum += neuron.bias;
    return neuron.actfunc(sum);
}


