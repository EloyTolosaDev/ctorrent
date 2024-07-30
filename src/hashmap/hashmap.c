#include "hashmap.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

uint64_t hash(char *str) {
  uint64_t v = 0;
  uint8_t idx = 1;
  while (*str) {
    v += (idx++) * (uint8_t)(*str++);
  }
  return v;
}

Node* new_node(void* value, HashmapType valueType, Node* next) {
  Node* n = malloc(sizeof(Node));
  n->valueType = valueType;
  n->value = value;
  n->next = next;
  return n;
}

/**
 * Reads the next key in the dictionary file
 */
char* __read_key(FILE* file) {
    char len[INT_MAX_DIGITS];
    uint8_t num_digits = 0;
    char c = 0;

    /**
     * First we read a number indicating the length of the key
     * we are going to read next
     */
    c = fgetc(file);
    while (isdigit(c)) {
        len[num_digits++] = c;
        c = fgetc(file);
    }
    len[num_digits] = '\0';

    /**
     * Read up to 'strlen' characters
     */
    size_t strlen = atoi(len);
    char* ret = malloc(sizeof(char) * strlen);
    size_t bytes = fread(ret, sizeof(char), strlen, file);
    if (bytes <= 0) {
        return NULL;
    }

    return ret;
}

void* __read_value(FILE* file, HashmapType type) {
  fprintf(stdout, "[__read_value] Reading a value of type %d\n", type);
  return NULL;
}

int32_t parse_hashmap(char* filepath, hashmap* torrent_hashmap) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "[parse_hashmap] error reading %s: %s\n", filepath, strerror(errno));
        return EXIT_FAILURE;
    }

    /**
     * Check if first file is a 'd'
     */
    if (fgetc(file) != 'd') {
        fprintf(stderr, "[parse_hashmap] file does not start with 'd'\n");
        return ERRBADFORMAT;
    }

    char c = 0;
    char* key;
    while ( (c = fgetc(file)) ) {
        switch (c) {
        case TYPE_DICT:
        case TYPE_LIST:
            __read_value(file, c);
            break;
        
        default:
            /**
             * Push back character read so we can re-read it into the key
             */
            ungetc(c, file);
            key = __read_key(file);
            if (!key) 
              return EXIT_SUCCESS;

            fprintf(stdout, "Key: %s\n", key);
            break;
        }
    }

    return EXIT_SUCCESS;
}