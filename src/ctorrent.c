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

  Hashmap* map = new_hashmap();
  if (parse_hashmap(filename, map) == EXIT_FAILURE) {
    fprintf(stderr, "[main] Error parsing hashmap.\n");
    return EXIT_FAILURE;
  }

  Hashmap* info =  (Hashmap*) get(map, "info")->value;
  fprintf(stdout, "Info hashmap: %p\n", info);
  void** l = (void**) get(info, "file-duration")->value;
  fprintf(stdout, "File duration list: %p\n", l);
  int64_t* fd = (int64_t*) l[0];
  fprintf(stdout, "File duration: %ld\n", *fd);
  

  return EXIT_SUCCESS;
}
