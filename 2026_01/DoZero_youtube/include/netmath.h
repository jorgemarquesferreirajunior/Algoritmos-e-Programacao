#ifndef NETMATH_H
#define NETMATH_H
#include <stdint.h>

float ident(float value);

float mse(float * out_true, float * out_pred, uint32_t samplesize);

#endif // NETMATH_H
