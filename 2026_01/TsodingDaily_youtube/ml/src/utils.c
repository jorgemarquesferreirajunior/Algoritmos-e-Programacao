#include "utils.h"
#include <stdlib.h>
#include <math.h>

float rand_float(void)
{
	return (float)rand() / (float)RAND_MAX;
}

float sigmoidf(float value){
    return 1.0f / (1.0f + expf(-value));
}
