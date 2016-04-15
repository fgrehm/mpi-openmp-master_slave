#include <stdio.h>
#include <stdlib.h>
// #include <limits.h>

#define TOTAL_ARRAYS 1000
#define TOTAL_NUMBERS 100000

int i, n;
unsigned long int** numbers;
int cmpfunc (const void * a, const void * b);

int main() {
  numbers = calloc(TOTAL_ARRAYS, sizeof(unsigned long int *));
  if (numbers == NULL) {
    fprintf(stderr, "calloc failed\n");
    return(-1);
  }

  printf("Preparing arrays...");
  for (i = 0; i < TOTAL_ARRAYS; i++) {
    numbers[i] = calloc(TOTAL_NUMBERS, sizeof(unsigned long int));
    if (numbers[i] == NULL) {
      fprintf(stderr, "calloc failed\n");
      return(-1);
    }

    for (n = TOTAL_NUMBERS; n > 0; n--) {
      numbers[i][TOTAL_NUMBERS-n-1] = n;
    }
  }
  printf("DONE\n");

  printf("Sorting arrays...\n");
  for (i = 0; i < TOTAL_ARRAYS; i++) {
    if (i % 100 == 0) printf("Processed %d\n", i);
    qsort(numbers[i], TOTAL_NUMBERS, sizeof(unsigned long int), cmpfunc);
  }
  printf("Done sorting\n");
}

int cmpfunc (const void * a, const void * b) {
  return ( *(unsigned long int*)a - *(unsigned long int*)b );
}
