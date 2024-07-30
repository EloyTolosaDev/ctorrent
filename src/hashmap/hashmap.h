#include <stdint.h>

#define DEFAULT_LEN 512

#define ERRBADFORMAT 1

/**
 * Hashmap values can be:
 *  - Another hashmap (dict)
 *  - A list
 *  - A string
 *  - An integer
 */
typedef enum hashmap_type {
    TYPE_DICT = 'd',
    TYPE_LIST = 'l', 
    TYPE_STRIING = 's',
    TYPE_INTEGER = 'i'
} HashmapType;

/**
 * A hashmap has to behave like a linked list in case of collisions
 */
struct node {
    char* value;
    struct node* next;
};
typedef struct node node_t;

typedef node_t *hashmap[DEFAULT_LEN];

node_t* new_node(char* value, struct node* next);

uint64_t hash(char *str);
int8_t put(hashmap *map, char *value);
int8_t update(hashmap *map, char *newvalue);

int32_t parse_hashmap(char* filepath, hashmap* torrent_hashmap);