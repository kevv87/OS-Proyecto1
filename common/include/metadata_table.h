#ifndef METADATA_TABLE_H
#define METADATA_TABLE_H


#include <semaphore.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


shared_struct_metadata * initialize_metadata_table();

#endif