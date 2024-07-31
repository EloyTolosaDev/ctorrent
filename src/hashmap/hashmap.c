#include "hashmap.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * To avoid warnings of "implicit declaration".
 * We will avoid them manually in the future.
 */
Hashmap* __read_dict(FILE* file);
void** __read_list(FILE* file);


uint64_t hash(char *str) {
  uint64_t v = 0;
  uint8_t idx = 1;
  while (*str) {
    v += (idx++) * (uint8_t)(*str++);
  }
  return v;
}

void put(Hashmap map, char* key, HashValueType valueType, void* value) {
  Node* n = new_node(value, valueType, NULL);
  uint64_t key_hash = hash(key) % DEFAULT_DICT_LEN;
  map[key_hash] = n;
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
    int strlen = atoi(len);
    char* ret = malloc(sizeof(char) * strlen + 1);

    /**
     * ERROR: See this link
     * https://stackoverflow.com/a/9389745
     */
    size_t bytes = fread(ret, sizeof(char), strlen, file);
    if (bytes <= 0) {
        fprintf(stderr, "\t[__read_key] %ld bytes read using fread\n", bytes);
        return NULL;
    }
    ret[strlen+1] = '\0';

    return ret;
}

int64_t __read_int(FILE* file) {
  char len[INT_MAX_DIGITS];
  uint8_t num_digits = 0;
  char c = 0;

  /**
   * First we read a number indicating the length of the key
   * we are going to read next
   */
  c = fgetc(file);
  while (isdigit(c) && c != EOVAL) {
      len[num_digits++] = c;
      c = fgetc(file);
  }
  
  /** 
   * Last character of a number must always be an 'e'.
   * Otherwise the file is not correctly formatted.
   */
  if (c != EOVAL) {
    fprintf(stderr, "[__read_int] Bad file format: integer not closed with terminating 'e' character");
    return -1;
  }
  len[num_digits] = '\0';

  /**
   * Read up to 'strlen' characters
   */
  return (int64_t) atoi(len);
}

Hashmap* __read_dict(FILE* file) {
  char c = 0;
  char* key = NULL;
  char* strval = NULL;
  void** list;
  int64_t ival = 0;
  Hashmap map;
  while ( (c = fgetc(file)) ) {
      switch (c) {
      case TYPE_DICT:
          key = NULL;
        break;
      case TYPE_LIST:

          fprintf(stdout, "[__read_dict] reading the list '%s'\n", key);
          list = __read_list(file);
          if (list == NULL) {
            fprintf(stderr, "[parse_hashmap] Error reading a list from the file\n");
            return NULL;
          }

          key = NULL;
        break;

      case TYPE_INTEGER:

          ival = __read_int(file);
          if (ival < 0) {
            fprintf(stderr, "[__read_dict] Error reading int from file");
            return NULL;
          }

          fprintf(stdout, "Correctly read %ld into the dict\n", ival);
          key = NULL;
        break;
      
      default:

        /**
         * Push back character read so we can re-read it into the str
         */
        if (ungetc(c, file) == EOF) {
          fprintf(stderr, "[__read_dict] Error ungetting char %c in file\n", c);
          return NULL;
        }
        
        /**
         * There's no way to tell if the string we're reading belongs to the
         * key or to the value of some key.
         * Thus, to know which one we're referring, we are going to follow 
         * this rules:
         *  - The first string read will **always** be a key.
         *  - The next thing we read will be a string again, or another thing.
         *  - If the next thing read after a key is a string, then that string corresponds
         *    to the value of that key.
         *  - If the next thing read after a key is **not** a string, then
         *    we read whatever it is, and the next string we know that corresponds
         *    to a key.
         */
        if (key == NULL) {
          key = __read_key(file);
          if (!key) {
            fprintf(stderr, "[__read_dict] Error reading key\n");
            return NULL;
          }
        } else {
          strval = __read_key(file);
          if (!strval) {
            fprintf(stderr, "[__read_dict] Error reading key\n");
            return NULL;
          }
        }

        /**
         * Add the new node to the hashmap only if there is an strval.
         * Otherwise it means that we read a key and we have to read the value,
         * which is not a string.
         */
        if (!strval) {
          fprintf(stderr, "[__read_dict] not a strval for key %s\n", key);
          continue;
        }

        /**
         * NOTE: This is not fucking necessary
         */
        if (!key) {
          fprintf(stderr, "[__read_dict] not a key\n");
          continue;
        }

        fprintf(stdout, "[__read_dict] Putting {%s: %s} into the hashnmap\n", key, strval);
        put(map, key, TYPE_STRING, strval);
        key = NULL, strval = NULL;

        break;
      }
  }
  return NULL;
}

void** __read_list(FILE* file) {
  void** list = malloc(LIST_DEFAULT_LEN * sizeof(void*));
  void** newlist;
  char* key;
  char c;
  int nelements = 0;
  int maxelements = LIST_DEFAULT_LEN;
  
  while ( (c = fgetc(file)) != EOVAL ) {

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

    switch (c) {
      case TYPE_LIST:
          fprintf(stdout, "\t[__read_list] reading a list\n");
          list[nelements++] = __read_list(file);
        break;
      
      case TYPE_DICT:
          fprintf(stdout, "\t[__read_list] reading a list\n");
          list[nelements++] = __read_dict(file);
        break;

      default:
          if (ungetc(c, file) == EOF) {
            fprintf(stderr, "\t[__read_list] error ungettinc char %c into file.\n", c);
            return NULL;
          }

          key = __read_key(file);
          fprintf(stdout, "\t[__read_list] read element %s into the list.\n", key);

          list[nelements++] = key;
        break;
    }
    
  }

  fprintf(stdout, "\t[__read_list] Finish reading the list\n");
  return list;
}



uint8_t parse_hashmap(char* filepath, Hashmap* hashmap) {
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
        return EXIT_FAILURE;
    }

    hashmap = __read_dict(file);

    return EXIT_SUCCESS;
}