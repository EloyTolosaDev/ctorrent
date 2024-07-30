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

Node* new_node(void* value, HashValueType valueType, Node* next) {
  Node* n = malloc(sizeof(Node));
  n->valueType = valueType;
  n->value = value;
  n->next = next;
  return n;
}

/**
 * Reads the next key in the dictionary file.
 * NOTE: Maybe change the name to "read_str" or "read_value"
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

    /**
     * ERROR: See this link
     * https://stackoverflow.com/a/9389745
     */
    size_t bytes = fread(ret, sizeof(char), strlen, file);
    if (bytes <= 0) {
        return NULL;
    }

    return ret;
}

char** __read_list(FILE* file) {
  char** list = malloc(LIST_DEFAULT_LEN * sizeof(char*));
  char** newlist;
  char* key;
  char c;
  int nelements = 0;
  int maxelements = LIST_DEFAULT_LEN;
  
  while ( (c = fgetc(file)) != EOVAL ) {

    fprintf(stdout, "\t[read_list] new character: %c\n", c);

    if (ungetc(c, file) == EOF) {
      fprintf(stderr, "[__read_list] error ungettinc char %c into file.\n", c);
      return NULL;
    }

    key = __read_key(file);
    fprintf(stdout, "\t[__read_list] read element %s into the list.\n", key);
    
    /**
     * Resize the list to double the size.
     * Increase the number of max elements by 2 times, copy the previous array
     * to the newly created array with the new size, and make the list point to
     * the new array. 
     * After that, free the memory from the new array.
     */
    if (nelements+1 == maxelements) {
      maxelements = 2*maxelements;
      newlist = malloc(maxelements * sizeof(char*));
      memcpy(newlist, list, (maxelements/2)*sizeof(char*));
      list = newlist;
      free(newlist);
    }

    list[nelements++] = key;
  }

  return list;
}


void* __read_value(FILE* file, HashValueType type) {
  fprintf(stdout, "[__read_value] Reading a value of type %d\n", type);
  return NULL;
}

int32_t parse_hashmap(char* filepath, Hashmap* torrent_hashmap) {
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
    char** list;
    while ( (c = fgetc(file)) ) {
        switch (c) {
        case TYPE_DICT:

          break;
        case TYPE_LIST:
            list = __read_list(file);
            if (list == NULL) {
              fprintf(stderr, "[parse_hashmap] Error reading a list from the file\n");
              return EXIT_FAILURE;
            }

            fprintf(stdout, "[parse_hashmap] List read from file!\n");
            for ( int i = 0; list[i] != NULL; i++ ) {
              fprintf(stdout, "[parse_hashmap] list[%d]=%s\n", i, list[i]);
            }
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