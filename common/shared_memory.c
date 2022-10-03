#include "include/shared_memory.h"
#include "include/types.h"

#include <sys/ipc.h>
#include <sys/shm.h>

/*/
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
 */
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static int get_id(char* block_name, int block_size)
{
    key_t key = ftok(block_name, 0);//Devuelve una llave basada en un path (BLOCK_NAME) y un id (en este caso 0) para hacer llamadas a RAM. Siempre que el par de datos sea el mismo devuelve lo mismo
    int id = shmget(key, block_size, 0666 | IPC_CREAT);//Share memory get devuleve el id asociado a una key_t
    printf("BLOCK_NAME: %s\n", block_name);
    printf("Key: %d\n", key);
    printf("id: %d\n", id);
    return id;
}
shared_struct* locate_shared_memory_block(char* block_name, int block_size)
{
    int id = get_id(block_name, block_size);
    shared_struct* res = (shared_struct*) shmat(id, NULL, 0);//Mapea la memoria compartida a un espacio en memoria
    return res;
}
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void *obtain_shared_pointer(size_t size, int shared_fd){
	void *ptr = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shared_fd, 0);
	if(ptr == MAP_FAILED){
		perror("MMAP failed");
		return NULL;
	}
	return ptr;
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