/*
 * fixed-pt-exp - Performance experiments
 *
 * Copyright © 2018 Bryce Harrington
 *
 * Released under gpl-2.0, read the file 'COPYING' for more information.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#define FIXEDPT_BITS (64)
#include "fixedpt/fixedptc.h"

#include "utils.h"

#define MAX_OPERATIONS (1024)

typedef enum _operation {
  OP_ADD = 0,  OP_SUB  = 1,
  OP_MUL = 2,  OP_DIV  = 3,
  OP_SQR = 4,  OP_SQRT = 5,

  OP_LAST
} operation_t;

const char *operation_symbols[] = {
  "+", "-",
  "×", "÷",
  "²", "√"
};

typedef void (i_operation)(int a, int b, int *result);
void i_add(int a, int b, int *result)                 { *result = a + b; }
void i_subtract(int a, int b, int *result)            { *result = a - b; }
void i_multiply(int a, int b, int *result)            { *result = a * b; }
void i_divide(int a, int b, int *result)              { *result = a / b; }
void i_square(int a, int UNUSED(b), int *result)      { *result = a * a; }
void i_square_root(int a, int UNUSED(b), int *result) { *result = a; /* TODO: Implement */ }
i_operation *i_operations[OP_LAST] = {
    i_add,
    i_subtract,
    i_multiply,
    i_divide,
    i_square,
    i_square_root
};

void f_add(float a, float b, float *result) { *result = a + b; }
void f_subtract() {}
void f_multiply() {}
void f_divide() {}
void f_square() {}
void f_square_root() {}

void d_add(double a, double b, double *result) { *result = a + b; }
void d_subtract() {}
void d_multiply() {}
void d_divide() {}
void d_square() {}
void d_square_root() {}

void q16_16_add(int a, int b, int *result) { *result = a + b; }
void q16_16_subtract() {}
void q16_16_multiply() {}
void q16_16_divide() {}
void q16_16_square() {}
void q16_16_square_root() {}

void q32_32_add(int a, int b, int *result) { *result = a + b; }
void q32_32_subtract() {}
void q32_32_multiply() {}
void q32_32_divide() {}
void q32_32_square() {}
void q32_32_square_root() {}

/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 *
 * https://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
 * License: Creative Commons Attribution-Share Alike
 */
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    assert(n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    assert(end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

/* Generate a sequence of operations to perform */
void generate_operations(int num, operation_t *operations, operation_t max_op) {

  // TODO: Do sets of ops in short sequences
  for (int i=0; i<num; i++) {
    operations[i] = randint(max_op);
  }
}

/* Generate a random array of numbers between 0 and max_val */
void generate_numbers(int num, int *numbers, int max_val) {
  for (int i=0; i<num; i++) {
    numbers[i] = randint(max_val);
  }
}

double
get_tick (void)
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return (double)now.tv_sec + (double)now.tv_usec / 1000000.0;
}

int main() {
  // Generate a random sequence of operations, and their opposites
  operation_t operations[MAX_OPERATIONS];
  int numbers[MAX_OPERATIONS];
  int num_operations = 100;
  int max_range = 42;
  int result = 0;
  double start_time, stop_time, run_time;

  srand(time(0));

  assert(num_operations <= MAX_OPERATIONS);
  generate_operations(num_operations, operations, OP_LAST);
  generate_numbers(num_operations, numbers, max_range);

  // Compare performance
  // Note where range errors occur
  start_time = get_tick();
  for (int i=0; i<num_operations; i++) {
    int op = operations[i];
    if (op == OP_SQR) {
      printf("%d%s", result, operation_symbols[op]);
    } else if (op == OP_SQRT) {
      printf("%s%d", operation_symbols[op], result);
    } else {
      printf("%d %s %d", result, operation_symbols[op], numbers[i]);
    }
    i_operations[op](result, numbers[i], &result);
    printf(" = %d\n", result);
  }
  stop_time = get_tick();
  run_time = stop_time - start_time;

  printf("Final result is %d in %f seconds\n", result, run_time);
}
