#include <stdint.h>

typedef struct TorrentInfoModeMultipleDict {
    uint32_t length; /** in bytes */
    char* md5sum;
    char* path[];
} TorrentInfoModeMultipleDict;

typedef struct TorrentInfoModeMultiple {
    char* dirname;
    TorrentInfoModeMultipleDict* files[];
} TorrentInfoModeMultiple;

typedef struct TorrentInfoModeSingle {
    char* name;
    uint32_t length;
    char* md5sum; /** OPTIONAL */
} TorrentInfoModeSingle;

typedef union TorrentInfoSpec  {
    torrent_info_single_file_t single_file;
    torrent_info_multiple_file_t multiple_file;
} TorrentInfoSpec;

typedef enum TorrentInfoMode {
    SINGLE_FILE, MULTI_FILE
} TorrentInfoMode;

typedef struct TorrentInfoCommon {
    uint32_t piece_length;
    uint32_t pieces;
    char private;               /** OPTIONAL. This is a boolean field! */
    enum torrent_type type;     /** Discriminate which type of torrent it is */
    union torrent_info_files files;
} TorrentInfoCommon;

typedef struct TorrentInfo {
    TorrentInfoCommon common;
    TorrentInfoSpec spec;
} TorrentInfo;

typedef struct Torrent {
    TorrentInfo* info;
    char* announce;
    char** announce_list; /** This is optional for backwards compatibility */
    char* creation_date;
    char* comment;
    char* created_by;
    char* encoding;
} Torrent;

int32_t parse_torrent(char* filepath, Torrent* torrent_file);