#include <stdint.h>
#include "../hashmap/hashmap.h"

typedef struct torrent_info_mf_file {
    uint32_t length; /** in bytes */
    char* md5sum;
    char* path[];
} torrent_info_mf_file_t;

typedef struct torrent_info_multiple_file {
    char* dirname;
    torrent_info_mf_file_t* files[];
} torrent_info_multiple_file_t;

typedef struct torrent_info_single_file {
    char* name;
    uint32_t length;
    char* md5sum; /** OPTIONAL */
} torrent_info_single_file_t;

union torrent_info_files {
    torrent_info_single_file_t single_file;
    torrent_info_multiple_file_t multiple_file;
};

enum torrent_type {
    single_file, multi_file
};

typedef struct torrent_info {
    uint32_t piece_length;
    uint32_t pieces;
    char private;               /** OPTIONAL. This is a boolean field! */
    enum torrent_type type;     /** Discriminate which type of torrent it is */
    union torrent_info_files files;
} torrent_info_t;

typedef struct torrent {
    torrent_info_t* info;
    char* announce;
    char** announce_list; /** This is optional for backwards compatibility */
    char* creation_date;
    char* comment;
    char* created_by;
    char* encoding;
} torrent_t;

int32_t parse_torrent(char *filepath, torrent_t *torrent_file);