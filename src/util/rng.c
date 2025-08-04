#include <stdlib.h>

#include "rng.h"

// Generates random within inclusive range
int rand_between(int min, int max) {
  return min + (rand()%(max-min+1));
}
