#include <stdint.h>

#define DEFAULT_LEN 512

#define ERRBADFORMAT 1

#define INT_MAX_DIGITS 20

/**
 * Hashmap values can be:
 *  - Another hashmap (dict)
 *  - A list
 *  - A string
 *  - An integer
 */
typedef enum HashmapType {
    TYPE_DICT = 'd',
    TYPE_LIST = 'l', 
    TYPE_STRIING = 's',
    TYPE_INTEGER = 'i'
} HashmapType;

/**
 * A node for a linked list.
 * The value of the linked list can be one of the mentioned in the
 * HashmapType enum.
 */
typedef struct Node {
    HashmapType valueType;
    void* value;
    struct Node* next;
} Node;

/**
 * A hashmap is an array of linked lists, just as the typical definition.
 */
typedef Node *Hashmap[DEFAULT_LEN];

Node* new_node(void* value, HashmapType valueType, Node* next);

uint64_t hash(char *str);
int8_t put(hashmap *map, char *value);
int8_t update(hashmap *map, char *newvalue);

int32_t parse_hashmap(char* filepath, hashmap* torrent_hashmap);