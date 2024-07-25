#include <stdint.h>

#define DEFAULT_LEN 512

/**
 * A hashmap has to behave like a linked list in case of collisions
 */
typedef struct node {
    char* value;
    struct node* next;
} node_t;

typedef node_t* hashmap[];

uint64_t hash(char *str);
int8_t put(hashmap *map, char *value);
int8_t update(hashmap *map, char *newvalue);