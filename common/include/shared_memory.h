#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/stat.h> // Mode constants
#include <fcntl.h> // O_ Constants
#include <unistd.h>

int *write_shared_memory(int shared_fd, int valueToWrite);
int obtain_shared_fd(char *name, bool createIfNull, int len);
void *obtain_shared_pointer(size_t size, int shared_fd);
int close_shared_pointer(void *mmap_ptr, int size);
int close_shared_memory(char *shm_name);


#endif //SHARED_MEMORY_H