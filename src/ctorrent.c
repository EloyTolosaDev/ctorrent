#include "hashmap/hashmap.h"
#include "bencoding/bencoding.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("[usage] ctorrent --file {filepath}\n");
    return EXIT_FAILURE;
  }

  char file = 0;
  char *filename;
  while (*argv) {
    if (strcmp(*argv++, "--file") == 0) {
      file = 1;
      filename = *argv;
    }
  }

  if (!file) {
    fprintf(stderr, "[ctorrent] required flag --file not provided\n");
    return EXIT_FAILURE;
  }

  if (filename == NULL) {
    fprintf(stderr, "[ctorrent] --file flag provided with no value\n");
    return EXIT_FAILURE;
  }

  Hashmap* map;
  parse_hashmap(filename, map);

  

  return EXIT_SUCCESS;
}
