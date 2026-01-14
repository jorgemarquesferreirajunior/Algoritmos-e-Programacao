#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <time.h>
#include <stdint.h>

typedef uint32_t u32;

float train[][2] = {
	{0,0},
	{1,2},
	{2,4},
	{3,6},
	{4,8},
};

#define train_count (sizeof(train) / sizeof(train[0]))

float cost(float w)
{
	float result = 0.0f;
	for (u32 i = 0; i < train_count; i++)
	{
		float x = train[i][0];
		float y = x * w;
		float d = y - train[i][1];
		result += d*d;
	}

	result /= train_count;
	return result;
}

int main()
{
	// srand(time(0));
	srand(69);
	float w = rand_float() * 10.0f;

	float eps = 1e-3;
	float rate = 1e-3;

	printf("%.5f\n", cost(w));
	for (u32 i = 0; i < 10; i++)
	{
		float dcost = (cost(w + eps) - cost(w)) / eps;
		w -= dcost * rate;
		printf("%.5f\n", cost(w));
	}

	return EXIT_SUCCESS;
}

