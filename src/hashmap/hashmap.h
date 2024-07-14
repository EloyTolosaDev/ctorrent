#include <stdint.h>

#define DEFAULT_LEN 512

typedef char *hashmap[];

uint64_t hash(char *str);
int8_t put(hashmap *map, char *value);
int8_t update(hashmap *map, char *newvalue);
