#include <stdio.h>
#include <stdlib.h>

#define TOTAL_ARRAYS  1000
#define TOTAL_NUMBERS 100000
#define MAX_NUMBER    TOTAL_ARRAYS * TOTAL_NUMBERS
#define T_NUMBER     int

T_NUMBER i, n;
int cmpfunc(const void * a, const void * b);
void debug_all_numbers(T_NUMBER **numbers);
void debug_numbers(T_NUMBER* numbers);

int main() {
  T_NUMBER **numbers = calloc(TOTAL_ARRAYS, sizeof(T_NUMBER *));
  printf("Preparing arrays...\n");
  for (i = 0; i < TOTAL_ARRAYS; i++) {
    numbers[i] = (T_NUMBER *)calloc(TOTAL_NUMBERS, sizeof(T_NUMBER));
    for (n = 0; n < TOTAL_NUMBERS; n++)
      numbers[i][n] = MAX_NUMBER - i * TOTAL_NUMBERS - n;
  }
  printf("DONE\n");

  debug_all_numbers(numbers);

  printf("Sorting arrays...\n");
  for (i = 0; i < TOTAL_ARRAYS; i++) {
    qsort(numbers[i], TOTAL_NUMBERS, sizeof(T_NUMBER), cmpfunc);
  }
  printf("Done sorting\n");

  debug_all_numbers(numbers);
}

void debug_all_numbers(T_NUMBER **numbers) {
  printf("First 5 arrays:\n");
  for (i = 0; i < 5; i++) {
    debug_numbers(numbers[i]);
  }
  printf(" ...\n");
  for (i = TOTAL_ARRAYS - 5; i < TOTAL_ARRAYS; i++) {
    debug_numbers(numbers[i]);
  }
}

void debug_numbers(T_NUMBER* numbers) {
  printf("  [ ");
  for (n = 0; n < 3; n++) {
    printf("%07d ", numbers[n]);
  }
  printf(" ... ");
  for (n = TOTAL_NUMBERS - 3; n < TOTAL_NUMBERS; n++) {
    printf("%07d ", numbers[n]);
  }
  printf("]\n");
}

int cmpfunc (const void * a, const void * b) {
  return ( *(T_NUMBER*)a - *(T_NUMBER*)b );
}
