/*
 * fixed-pt-exp - Performance experiments
 *
 * Copyright © 2018 Bryce Harrington
 *
 * Released under gpl-2.0, read the file 'COPYING' for more information.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define FIXEDPT_BITS (64)
#include "fixedpt/fixedptc.h"

#define MAX_OPERATIONS (1024)

// TODO: Define these for all data types
void add() {}
void subtract() {}
void multiply() {}
void divide() {}
void square() {}
void square_root() {}

typedef enum _operation {
  OP_SQRT = -3,
  OP_DIV = -2,
  OP_SUB = -1,

  OP_NONE = 0,

  OP_ADD = 1,
  OP_MUL = 2,
  OP_SQR = 3,

  OP_LAST = 4
} operation_t;

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

int main() {
  // Generate a random sequence of operations, and their opposites
  operation_t operations[MAX_OPERATIONS];
  int num_operations = 10;

  srand(time(0));

  assert(num_operations <= MAX_OPERATIONS);
  for (int i=0; i<num_operations; i++) {
    operations[i] = randint(OP_LAST);
  }

  // Do sets of ops in short sequences

  // Run for several numerical ranges - small, medium, large, huge

  // Compare performance
  // Note where range errors occur
  for (int i=0; i<num_operations; i++) {
    printf("Hello operation %d\n", operations[i]);
  }
}
