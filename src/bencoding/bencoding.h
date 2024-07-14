#include <stdint.h>

typedef struct torrent {

} torrent_t;

#define dict 'd'
#define list 'l'
#define int 'i'
#define end 'e'

int32_t parse_torrent(char *filepath, torrent_t *torrent_file);
