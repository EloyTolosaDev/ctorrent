#include "hashmap/hashmap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc < 0) {
    printf("[usage] ctorrent --file {filepath}\n");
    return EXIT_FAILURE;
  }

  uint8_t file = 0;
  char *filename;
  while (*argv) {
    if (strcmp(*argv++, "file") == 0) {
      file = 1;
      filename = *argv;
    }
  }

  if (file) {
    printf("File to process is %s\n", filename);
  }

  printf("[debug] testing hashing functions...");
  printf("[hash] Hello=%lu\n", hash("hello"));

  return EXIT_SUCCESS;
}
