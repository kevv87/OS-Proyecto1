//
// Created by majinloop on 03/10/22.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H
// encoder.c
#define SHM_CHUNK "/shm_chunk"
#define STB_IMAGE_IMPLEMENTATION
#define KB 1024
#define INPUT_LIMIT 1*KB
// metadata_table.h
#define MAX_METADATA_NODES 15
#define METADATA_BLOCK_SIZE sizeof(shared_struct_metadata)
#define METADATA_BLOCK_NAME "Shared_memory_metadata"

#endif //CONSTANTS_H
