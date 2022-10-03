#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h> // Mode constants
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h> // O_ Constants
#include <unistd.h>

#include "types.h"

//void *write_shared_memory(void *dst_shared_ptr, void *src_ptr, size_t size);
//void *read_shared_memory(void *dst_ptr, void *src_shm_ptr, size_t size);

void *obtain_shm_pointer(int shmid, void * map_ptr);

void sem_down(Statistic_t *statistic_ptr, sem_t semaphore);

void * write_shared_memory(void *dst_shared_ptr, void *src_ptr, size_t size);

int get_id(char * name, size_t size);

int close_shm_ptr(int shmid, void * shm_ptr);


#endif //SHARED_MEMORY_H