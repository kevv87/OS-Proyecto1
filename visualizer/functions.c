#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

#include "lib/structs.h"
#include "lib/constants.h"

void create_metadata() {

    int fd_metadata = shm_open(METADATA_MEMORY, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd_metadata == -1) {

        fd_metadata = shm_open(METADATA_MEMORY, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        ftruncate(fd_metadata, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)));

    }

    //metadata_node_t *metada_ptr = mmap(NULL, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

}

metadata_node_t *create_metadata_node(int total_pixeles, int width, int heigth) {

    metadata_node_t *node = malloc(sizeof(metadata_node_t));

    node -> total_pixeles = total_pixeles;
    node -> width = width;
    node -> heigth = heigth;
    node -> has_decoder = 1;

    sem_t semaphore;

    sem_init(&semaphore, 1, 0);

    node -> image_semaphore = semaphore;

    return node;

}

int add_metadata_node(metadata_node_t *metada_ptr, metadata_node_t *node) {

    for(int i = 0; i < METADATA_SIZE; i++) {

        if(&(metada_ptr[i]) == 0) {

            memcpy(&metada_ptr[i], node, sizeof(metadata_node_t));

            free(node);

            return i;

        }

    }

}



void print_buffer(void *ptr, size_t size) {
    char * src_ptr = ptr;
    for(int i = 0; i < size; i+= sizeof(char)) {
        printf("val:%d ", *(src_ptr+i));
        if(i%4 ==0)
            printf("\n");
    }
}













int main() {

    printf("Main\n");

    create_metadata();

    int fd_metadata = shm_open(METADATA_MEMORY, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    metadata_node_t *metadata_ptr = mmap(NULL, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)), PROT_READ | PROT_WRITE, MAP_SHARED, fd_metadata, 0);

    //metadata_node_t *node = create_metadata_node(1000, 50, 70);

    //add_metadata_node(metadata_ptr + sizeof(sem_t), node);

    //printf("%d\n\n", (metada_ptr + sizeof(sem_t))[0].);


    //sem_t semaphore;
    //metadata_ptr[0] = semaphore;
    




    metadata_node_t node;

    node.total_pixeles = 1000;
    node.width = 200;
    node.heigth = 300;
    node.has_decoder = 1;

    (metadata_ptr + sizeof(sem_t))[0] = node;

    //print_buffer(metadata_ptr, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)));

    //printf("%d\n", (metadata_ptr + sizeof(sem_t))[0].width);

    printf("%d\n", metadata_ptr[0].width);


    munmap(metadata_ptr, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)));

    unlink(METADATA_MEMORY);

    close(fd_metadata);

}