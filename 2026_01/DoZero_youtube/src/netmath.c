#include "netmath.h"
#include <math.h>
#include <stdint.h>

float ident(float value){
    return value;
}

float sigmoid(float value){
    return 1 / (1 + exp(-value));
}

float mse(float * out_true, float * out_pred, uint32_t samplesize){
    float sum = 0;
    for (uint32_t k = 0; k < samplesize; k ++) {
        sum += (float)pow(out_true[k] - out_pred[k], 2);
    }
    return sum / samplesize;
}
