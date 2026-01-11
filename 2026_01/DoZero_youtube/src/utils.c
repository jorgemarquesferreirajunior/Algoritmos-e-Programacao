#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


float randomize(float min, float max){
    return min + ((float)rand() / (float)RAND_MAX * (max - min));
}

float **mallocmatrix(uint32_t nlins, uint32_t ncols){
    float **matrix = (float **)malloc(sizeof(float *) * nlins);
    for (uint32_t nl = 0; nl < nlins; nl++) {
        matrix[nl] = (float *)malloc(sizeof(float) * ncols);
    }
    return matrix;
}

void pause_and_clear(void) {
    printf("Pressione qualquer tecla para continuar...");
    getchar();               // espera ENTER ou qualquer tecla + ENTER
    printf("\033[2J\033[H"); // clear
}
