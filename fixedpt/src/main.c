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
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

//#define DEBUG (1)
#define MAX_OPERATIONS (100)
#define MAX_RANGE (8)
#define INITIAL_VALUE (2)

// Set this to 32 or 64 to test the two different number types
#define FIXEDPT_BITS (64)
#include "fixedpt/fixedptc.h"

#include "utils.h"

/**
 * @brief Fast Square root algorithm, with rounding
 *
 * This does arithmetic rounding of the result. That is, if the real answer
 * would have a fractional part of 0.5 or greater, the result is rounded up to
 * the next integer.
 *   - SquareRootRounded(2) --> 1
 *   - SquareRootRounded(3) --> 2
 *   - SquareRootRounded(4) --> 2
 *   - SquareRootRounded(6) --> 2
 *   - SquareRootRounded(7) --> 3
 *   - SquareRootRounded(8) --> 3
 *   - SquareRootRounded(9) --> 3
 *
 * Ref. https://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
 *
 * @param[in] a_nInput - unsigned integer for which to find the square root
 *
 * @return Integer square root of the input value.
 */
uint32_t i_square_root_rounded(uint32_t a_nInput) {
  uint32_t op  = a_nInput;
  uint32_t res = 0;
  uint32_t one = 1uL << 30;
  // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type

  // "one" starts at the highest power of four <= than the argument.
  while (one > op) {
    one >>= 2;
  }

  while (one != 0) {
    if (op >= res + one) {
      op = op - (res + one);
      res = res +  2 * one;
    }
    res >>= 1;
    one >>= 2;
  }

  /* Do arithmetic rounding to nearest integer */
  if (op > res) {
    res++;
  }

  return res;
}

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
void i_square_root(int a, int UNUSED(b), int *result) { *result = i_square_root_rounded(abs(a)); }
i_operation *i_operations[OP_LAST] = {
    i_add,
    i_subtract,
    i_multiply,
    i_divide,
    i_square,
    i_square_root
};

typedef void (f_operation)(float a, float b, float *result);
void f_add(float a, float b, float *result)                    { *result = a + b; }
void f_subtract(float a, float b, float *result)               { *result = a - b; }
void f_multiply(float a, float b, float *result)               { *result = a * b; }
void f_divide(float a, float b, float *result)                 { *result = a / b; }
void f_square(float a, float UNUSED(b), float *result)         { *result = a * a; }
void f_square_root(float a, float UNUSED(b), float *result)    { *result = sqrt(fabs(a)); }
f_operation *f_operations[OP_LAST] = {
    f_add,
    f_subtract,
    f_multiply,
    f_divide,
    f_square,
    f_square_root
};

typedef void (d_operation)(double a, double b, double *result);
void d_add(double a, double b, double *result)                 { *result = a + b; }
void d_subtract(double a, double b, double *result)            { *result = a - b; }
void d_multiply(double a, double b, double *result)            { *result = a * b; }
void d_divide(double a, double b, double *result)              { *result = a / b; }
void d_square(double a, double UNUSED(b), double *result)      { *result = a * a; }
void d_square_root(double a, double UNUSED(b), double *result) { *result = sqrt(fabs(a)); }
d_operation *d_operations[OP_LAST] = {
    d_add,
    d_subtract,
    d_multiply,
    d_divide,
    d_square,
    d_square_root
};

typedef void (fp_operation)(fixedpt a, fixedpt b, fixedpt *result);
void fp_add(fixedpt a, fixedpt b, fixedpt *result) { *result = fixedpt_add(a, b); }
void fp_subtract(fixedpt a, fixedpt b, fixedpt *result) { *result = fixedpt_sub(a, b); }
void fp_multiply(fixedpt a, fixedpt b, fixedpt *result) { *result = fixedpt_mul(a, b); }
void fp_divide(fixedpt a, fixedpt b, fixedpt *result) { *result = fixedpt_div(a, b); }
void fp_square(fixedpt a, fixedpt UNUSED(b), fixedpt *result) { *result = fixedpt_mul(a, a); }
void fp_square_root(fixedpt a, fixedpt UNUSED(b), fixedpt *result) {
  *result = fixedpt_sqrt(fixedpt_abs(a));
}
fp_operation *fp_operations[OP_LAST] = {
    fp_add,
    fp_subtract,
    fp_multiply,
    fp_divide,
    fp_square,
    fp_square_root
};


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

/* Generate a random array of non-zero numbers between 1 and max_val */
void generate_numbers(int num, int *numbers, int max_val) {
  for (int i=0; i<num; i++) {
    numbers[i] = randint(max_val-1) + 1;
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
  int num_operations = MAX_OPERATIONS;
  double start_time, stop_time, run_time;

  srand(time(0));

  assert(num_operations <= MAX_OPERATIONS);
  generate_operations(num_operations, operations, OP_LAST);
  generate_numbers(num_operations, numbers, MAX_RANGE);

  // INTEGER
  // -------
  {
    int result = INITIAL_VALUE;
    start_time = get_tick();
    for (int i=0; i<num_operations; i++) {
      int op = operations[i];
#ifdef DEBUG
      if (op == OP_SQR) {
	printf("%d%s", result, operation_symbols[op]);
      } else if (op == OP_SQRT) {
	printf("%s%d", operation_symbols[op], result);
      } else {
	printf("%d %s %d", result, operation_symbols[op], numbers[i]);
      }
      fflush(stdout);
#endif
      i_operations[op](result, numbers[i], &result);
#ifdef DEBUG
      printf(" = %d\n", result);
#endif
    }
    stop_time = get_tick();
    run_time = stop_time - start_time;

    printf("Integer result is %d in %f msecs\n\n", result, run_time*1000);
  }

  // FLOAT
  // -----
  {
    float result = INITIAL_VALUE;
    start_time = get_tick();
    for (int i=0; i<num_operations; i++) {
      int op = operations[i];
#ifdef DEBUG
      if (op == OP_SQR) {
	printf("%f%s", result, operation_symbols[op]);
      } else if (op == OP_SQRT) {
	printf("%s%f", operation_symbols[op], result);
      } else {
	printf("%f %s %d", result, operation_symbols[op], numbers[i]);
      }
      fflush(stdout);
#endif
      f_operations[op](result, (float)numbers[i], &result);
#ifdef DEBUG
      printf(" = %f\n", result);
#endif
    }
    stop_time = get_tick();
    run_time = stop_time - start_time;

    printf("Float result is %f in %f msecs\n\n", result, run_time*1000);
  }

  // DOUBLE
  {
    double result = INITIAL_VALUE;
    start_time = get_tick();
    for (int i=0; i<num_operations; i++) {
      int op = operations[i];
#ifdef DEBUG
      if (op == OP_SQR) {
	printf("%f%s", result, operation_symbols[op]);
      } else if (op == OP_SQRT) {
	printf("%s%f", operation_symbols[op], result);
      } else {
	printf("%f %s %d", result, operation_symbols[op], numbers[i]);
      }
      fflush(stdout);
#endif
      d_operations[op](result, (double)numbers[i], &result);
#ifdef DEBUG
      printf(" = %f\n", result);
#endif
    }
    stop_time = get_tick();
    run_time = stop_time - start_time;

    printf("Double precision result is %f in %f msecs\n\n", result, run_time*1000);
  }

  // FIXED-POINT
  {
    fixedpt result = fixedpt_fromint(INITIAL_VALUE);
    start_time = get_tick();
    for (int i=0; i<num_operations; i++) {
      int op = operations[i];
#ifdef DEBUG
      if (op == OP_SQR) {
	printf("%s%s", fixedpt_cstr(result, -1), operation_symbols[op]);
      } else if (op == OP_SQRT) {
	printf("%s%s", operation_symbols[op], fixedpt_cstr(result, -1));
      } else {
	printf("%s %s %d", fixedpt_cstr(result, -1), operation_symbols[op], numbers[i]);
      }
      fflush(stdout);
#endif
      fp_operations[op](result, fixedpt_fromint(numbers[i]), &result);
#ifdef DEBUG
      printf(" = %s\n", fixedpt_cstr(result, -1));
#endif
    }
    stop_time = get_tick();
    run_time = stop_time - start_time;

    printf("Fixed point result is %s in %f msecs\n\n", fixedpt_cstr(result, -1), run_time*1000);
  }

}
