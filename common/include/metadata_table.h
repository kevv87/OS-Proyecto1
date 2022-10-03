#ifndef METADATA_TABLE_H
#define METADATA_TABLE_H


#include <semaphore.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


#define MAX_METADATA_NODES 15
#define BLOCK_SIZE sizeof(shared_struct_metadata)
#define BLOCK_NAME "Shared_memory_metadata"


typedef struct MetadataEntry
{
    int total_pixeles;
    int width, height;
    int has_decoder;
    sem_t image_semaphore;
} MetadataEntry_t;


typedef struct
{
    MetadataEntry_t metadata_array[MAX_METADATA_NODES];
    sem_t sem_metadata;
}shared_struct_metadata;


shared_struct_metadata * initialize_metadata_table();

#endif