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

void *write_shared_memory(void *dst_shared_ptr, void *src_ptr, size_t size);
void *read_shared_memory(void *dst_ptr, void *src_shm_ptr, size_t size);

void *obtain_shm_pointer(char * name, size_t size);
int close_shm_ptr(void * shm_ptr);

int get_id(char * name, size_t size);
int close_shared_pointer(void *mmap_ptr, int size);
int close_shared_memory(char *shm_name);


#endif //SHARED_MEMORY_H