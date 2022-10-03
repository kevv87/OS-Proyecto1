#ifndef METADATA_TABLE_H
#define METADATA_TABLE_H

#define METADATA_SHM_NAME "/metadata"

#include <semaphore.h>

#include "include/shared_memory.h"

typedef struct MetadataEntry{
    int total_pixeles;
    int width, height;
    int has_decoder;
    sem_t image_semaphore;
} MetadataEntry_t;

MetadataEntry_t * initialize_metadata_table();

#endif