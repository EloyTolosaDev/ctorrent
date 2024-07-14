#include "hashmap.h"

uint64_t hash(char *str) {
  uint64_t v = 0;
  uint8_t idx = 1;
  while (*str) {
    v += (idx++) * (uint8_t)(*str++);
  }
  return v;
}
