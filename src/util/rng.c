#include <stdlib.h>

#include "rng.h"

int rand_between(int min, int max) {
  return min + (rand()%(max-min+1));
}
