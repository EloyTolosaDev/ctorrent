#include "hashmap.h"

#define STR_EQ(s1, s2) strcmp(s1, s2) == 0

Hashmap* __read_dict(FILE* file);
int64_t* __read_int(FILE* file);
void** __read_list(FILE* file);

char* __to_string(void* value, HashValueType type);


Hashmap* new_hashmap() {
  Hashmap* map = malloc(sizeof(Hashmap));
  for (int i = 0; i < DEFAULT_DICT_LEN; i++)
    map->arr[i] = NULL;
  return map;
}

uint64_t hash(char *str) {
  uint64_t v = 0;
  uint8_t idx = 1;
  while (*str) {
    v += (idx++) * (uint8_t)(*str++);
  }
  return v;
}

char* __to_string(void* value, HashValueType type) {
  char* val;
  switch (type) {
    case TYPE_STRING:
      val = (char*) value;
      break;
    case TYPE_INTEGER:
      val = malloc(sizeof(char)*INT_MAX_DIGITS);
      sprintf(val, "%p", (uint64_t*) value);
      break;
    case TYPE_LIST:
      val = "[to be implemented] a list";
      break;
    case TYPE_DICT:
      val = "[to be implemented] a dict";
      break;
    /** 
     * TODO: Implement DICTS later. It has to be recursive
     */
  }
  return val;
}

void put(Hashmap* map, char* key, HashValueType valueType, void* value) {
  Node* n = new_node(value, valueType, NULL);
  uint64_t key_hash = hash(key) % DEFAULT_DICT_LEN;
  fprintf(stdout, "[put] Putting {%s: %s} into the hashnmap at [%ld]\n", key, __to_string(n->value, n->valueType), key_hash);
  /**
   * Check for collisions.
   * In case of collisions, add the node at the end of the list.
   */
  if (map->arr[key_hash] == NULL) {
    map->arr[key_hash] = n;
  } else {
    Node* iter = map->arr[key_hash];
    while (iter->next != NULL)
      iter = iter->next;

    iter->next = n;
  }
}

/**
 * The caller of this function must check for nu
 */
Node* get(Hashmap* map, char* key) {
  return map->arr[hash(key)%DEFAULT_DICT_LEN];
}

Node* new_node(void* value, HashValueType valueType, Node* next) {
  Node* n = malloc(sizeof(Node));
  if (!n) {
    fprintf(stderr, "[new_node] error allocating memory for Node*\n");
    return NULL;
  }

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
    int str_len = atoi(len);

    size_t bytes_to_read = sizeof(char) * str_len;
    char* ret = malloc(bytes_to_read + 1);

    fread(ret, sizeof(char), str_len, file);

    ret[bytes_to_read] = '\0';

    return ret;
}

int64_t* __read_int(FILE* file) {
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
    return NULL;
  }
  len[num_digits] = '\0';

  /**
   * Read up to 'strlen' characters
   */
  int64_t* ret = malloc(sizeof(int64_t));
  *ret = atol(len);
  return ret;
}

Hashmap* __read_dict(FILE* file) {

  char c = 0;
  char* key = NULL;
  char* strval = NULL;
  void** list;
  int64_t* ival;

  Hashmap* map = new_hashmap();
  Hashmap* new = new_hashmap();

  c = fgetc(file);
  while ( c != EOVAL && c != EOF ) {
      switch (c) {
      case TYPE_DICT:

          new = __read_dict(file);

          put(map, key, TYPE_DICT, new);

          key = NULL;
        break;
      case TYPE_LIST:

          list = __read_list(file);
          if (list == NULL) {
            fprintf(stderr, "[__read_dict] Error reading a list from the file\n");
            return NULL;
          }

          put(map, key, TYPE_LIST, list);

          key = NULL;
        break;

      case TYPE_INTEGER:

          ival = __read_int(file);
          if (ival == NULL) {
            fprintf(stderr, "[__read_dict] Error reading int from file for key %s\n", key);
            return NULL;
          }

          fprintf(stdout, "Value of ival: %p\n", &ival);

          /**
           * Once we read the int, we put it into the dict
           */
          put(map, key, TYPE_INTEGER, &ival);

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
          if (key == NULL) {
            fprintf(stderr, "[__read_dict] Error reading key\n");
            return NULL;
          }
        } else {
          strval = __read_key(file);
          if (strval == NULL) {
            fprintf(stderr, "[__read_dict] Error reading key\n");
            return NULL;
          }
        }

        /**
         * Add the new node to the hashmap only if there is an strval.
         * Otherwise it means that we read a key and we have to read the value,
         * which is not a string.
         */
        if (!strval)
          break;

        put(map, key, TYPE_STRING, strval);
        key = NULL, strval = NULL;

        break;
      }

      c = fgetc(file);
  }

  if (c == EOF) {
    fprintf(stderr, "[__read_dict] error reading dict: EOF reached before corresponding EOVAL");
    return NULL;
  } 

  return map;
}

void** __read_list(FILE* file) {

  void** list = malloc(LIST_DEFAULT_LEN * sizeof(void*));
  void** newlist;
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
          list[nelements++] = __read_list(file);
        break;
      
      case TYPE_DICT:
          list[nelements++] = __read_dict(file);
        break;

      case TYPE_INTEGER:
          list[nelements++] = __read_int(file);
        break;

      default:
          if (ungetc(c, file) == EOF) {
            fprintf(stderr, "\t[__read_list] error ungettinc char %c into file.\n", c);
            return NULL;
          }

          list[nelements++] = __read_key(file);
        break;
    }
    
  }

  return list;
}

uint8_t parse_hashmap(char* filepath, Hashmap *hashmap) {
    FILE* file = fopen(filepath, "rb");
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

    // ungetc()
    *hashmap = *__read_dict(file);
    if (!hashmap) {
      fprintf(stderr, "[parse_hashmap] error reading dict: hashmap is null\n");
      return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}