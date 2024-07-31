#include <stdint.h>

#define DEFAULT_DICT_LEN 512

#define ERRBADFORMAT 1

#define INT_MAX_DIGITS 20

#define LIST_DEFAULT_LEN 32

/**
 * Marks the end of any value in a bencoding element
 */
#define EOVAL 'e'

/**
 * Hashmap values can be:
 *  - Another hashmap (dict)
 *  - A list
 *  - A string
 *  - An integer
 */
typedef enum HashValueType {
    TYPE_DICT = 'd',
    TYPE_LIST = 'l', 
    TYPE_STRING = 's',
    TYPE_INTEGER = 'i'
} HashValueType;

/**
 * A node for a linked list.
 * The value of the linked list can be one of the mentioned in the
 * HashmapType enum.
 */
typedef struct Node {
    HashValueType valueType;
    void* value;
    struct Node* next;
} Node;

/**
 * A hashmap is an array of linked lists, just as the typical definition.
 */
typedef Node *Hashmap[DEFAULT_DICT_LEN];

Node* new_node(void* value, HashValueType valueType, Node* next);

uint64_t hash(char *str);
void put(Hashmap map, char* key, HashValueType valueType, void* value);
int8_t update(Hashmap map, HashValueType newValueType, void* newValue);

uint8_t parse_hashmap(char* filepath, Hashmap* hashmap);