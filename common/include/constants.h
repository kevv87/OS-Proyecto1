//
// Created by majinloop on 03/10/22.
//

#include "types.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

// encoder.c
#define SHM_CHUNK "/shm_chunk"
#define STB_IMAGE_IMPLEMENTATION
#define KB 1024
#define INPUT_LIMIT 1*KB

// metadata_table.h
#define MAX_METADATA_NODES 15

#define MAX_SEM 15
#define BLOCK_SIZE sizeof(shared_struct)

#define CHUNK_LIST_PATH "../../shared_memory_paths/chunk_list_path"
#define CHUNK_BLOCK_SIZE sizeof(Statistic_t) // **************** falta

#define METADATA_PATH "../../shared_memory_paths/metadata_path"
#define METADATA_BLOCK_SIZE sizeof(Metadata_Table_t)

#define STATISTIC_PATH "../../shared_memory_paths/statistic_path"
#define STATISTIC_BLOCK_SIZE sizeof(Statistic_t)












#endif //CONSTANTS_H
