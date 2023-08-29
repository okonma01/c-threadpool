#include <stdio.h>
#include <stdlib.h>
#include "bruteforce.h"

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Usage: %s NUM_THREADS ENCRYPTED\n", argv[0]);
    return -1;
  }

  unsigned long num_threads = strtoul(argv[1], NULL, 10);
  char *encrypted = argv[2];

  bruteforce(num_threads, encrypted);
  
  return 0;
}
