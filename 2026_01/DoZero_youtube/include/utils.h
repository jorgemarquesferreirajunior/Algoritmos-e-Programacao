#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <stdint.h>

float randomize(float min, float max);
float **mallocmatrix(uint32_t nlins, uint32_t ncols);
void pause_and_clear(void);
#endif // UTILS_H
