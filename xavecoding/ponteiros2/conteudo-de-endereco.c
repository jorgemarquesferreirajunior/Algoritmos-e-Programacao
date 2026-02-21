#include <stdio.h>
#include <stdlib.h>

int main(void) {
  float z = 2.45;
  float *fp;

  fp = &z;

  printf("&z = %p, *&z = %f\n", &z, *&z);
  printf("fp = %p, *fp = %f\n", fp, *fp);
  printf("**&fp = %f\n", **&fp);

  return EXIT_SUCCESS;
}
