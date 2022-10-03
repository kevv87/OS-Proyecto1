#ifndef METADATA_TABLE_H
#define METADATA_TABLE_H

#include <semaphore.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int get_id(char* block_name, int block_size);
Metadata_Table_t* locate_metadata_table(char* block_name, int block_size);
shared_struct_metadata * initialize_metadata_table();

#endif