#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int a = 10;

  printf("%-25s = %ld bytes\n", "sizeof(char)", sizeof(char));
  printf("%-25s = %ld bytes\n", "sizeof(short)", sizeof(short));
  printf("%-25s = %ld bytes\n", "sizeof(a)", sizeof(a));
  printf("%-25s = %ld bytes\n", "sizeof(int)", sizeof(int));
  printf("%-25s = %ld bytes\n", "sizeof(float)", sizeof(float));
  printf("%-25s = %ld bytes\n", "sizeof(long)", sizeof(long));
  printf("%-25s = %ld bytes\n", "sizeof(unsigned long)", sizeof(unsigned long));
  printf("%-25s = %ld bytes\n", "sizeof(double)", sizeof(double));
  printf("%-25s = %ld bytes\n", "sizeof(void *)", sizeof(void *));
  printf("%-25s = %ld bytes\n", "sizeof(int *)", sizeof(int *));
  printf("%-25s = %ld bytes\n", "sizeof(int **)", sizeof(int **));
  printf("%-25s = %ld bytes\n", "sizeof(int ***)", sizeof(int ***));
  printf("%-25s = %ld bytes\n", "sizeof(float *)", sizeof(float *));

  return EXIT_SUCCESS;
}
