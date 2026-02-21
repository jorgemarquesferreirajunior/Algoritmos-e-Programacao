#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int a, *p1;

  p1 = NULL;

  printf(" 'a' = %d,'&a' = %lu\n", a, (unsigned long)&a);
  a = 10;
  printf(" 'a' = 10;\n\n");
  printf(" 'a' = %d,'&a' = %lu\n\n", a, (unsigned long)&a);

  printf(" 'p1' = %p\n", p1);
  p1 = &a;
  printf(" 'p1' = &a\n\n");
  printf(" '*p1' = %d, 'p1' = %lu, '&p1' = %lu\n\n", *p1, (unsigned long)p1,
         (unsigned long)&p1);
  int *p2;
  p2 = p1;

  *p2 = 3;
  printf(" '*p12 = %d, 'p2' = %lu, '&p2' = %lu\n\n", *p2, (unsigned long)p2,
         (unsigned long)&p2);
  printf(" 'a' = %d,'&a' = %lu\n\n", a, (unsigned long)&a);

  return EXIT_SUCCESS;
}
