#include "include/shared_memory.h"

int obtain_shared_fd(char *name, bool createIfNull, int len){
	int shared_fd;

	shared_fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);

	if (shared_fd == -1){
		shared_fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if(shared_fd == -1 && createIfNull){
			perror("shm_open failed");
			return -1;
		}
	}

	if(createIfNull){
		if (ftruncate(shared_fd, len) == -1){
			perror("ftruncate failed");
			return -1;
		}
	}

	return shared_fd;
}

void *obtain_shared_pointer(size_t size, int shared_fd){
	void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shared_fd, 0);
	if(ptr == MAP_FAILED){
		perror("MMAP failed");
		return NULL;
	}
	return ptr;
}

int get_id(char * name, size_t size){
    key_t key = ftok(name, 0);
    return shmget(key, size, 0664 | IPC_CREAT);
}

void *obtain_shm_pointer(char * name, size_t size){
    int id = get_id(name, size);
    void * shm_ptr = shmat(id, NULL, 0);
    if(shm_ptr==NULL){
        perror("shmat failed");
        return NULL;
    }
    return shm_ptr;
}

int close_shm_ptr(void * shm_ptr){
    if(shmdt(shm_ptr) == -1){
        perror("shmdt failed");
        return -1;
    }
    return 0;
}

int close_shared_pointer(void *mmap_ptr, int size){
	int errno = munmap(mmap_ptr, size);

	if(errno == -1){
		perror("munmap failed");
	}
	return errno;
}

int close_shared_memory(char *shm_name){
	int errno = shm_unlink(shm_name);
	if( errno == -1){
		perror("shm_unlink failed");
	}
	return errno;
}

void *write_shared_memory(void *dst_shm_ptr, void *src_ptr, size_t size){
    return memmove(dst_shm_ptr, src_ptr, size);
}

void *read_shared_memory(void *dst_ptr, void *src_shm_ptr, size_t size){
    return memmove(dst_ptr, src_shm_ptr, size);
}