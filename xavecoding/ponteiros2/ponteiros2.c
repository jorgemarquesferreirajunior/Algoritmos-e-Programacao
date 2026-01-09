#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int a, *p1, **p2;

  a = 5;

  p1 = &a;
  p2 = &p1;
  printf("%-5s = %p, a = %d\n", "&a", &a, a);
  printf("%-5s = %p\n", "p1", p1);
  printf("%-5s = %p\n", "*p2", *p2);

  printf("********************\nAtribuindo um valor para  **p2:\n");
  **p2 = 235;
  printf("%-5s = 5\n", "**p2");
  printf("********************\n");
  printf("%-5s = %d\n", "*p1", *p1);
  printf("%-5s = %p, a = %d\n", "&a", &a, a);
  printf("********************\nAtribuindo um valor para *p1:\n");
  *p1 = 17;
  printf("%-5s = %d\n", "*p1", *p1);
  printf("********************\n");
  printf("%-5s = %p, a = %d\n", "&a", &a, a);
  printf("%-5s = 17\n", "*p1");

  return EXIT_SUCCESS;
}
