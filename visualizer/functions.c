#include <stdio.h>
#include <semaphore.h>

#include "lib/structs.h"
#include "lib/constants.h"

void create_metadata() {

    int fd_metadata = shm_open(METADATA_MEMORY, O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

    if(fd_metadata == -1) {

        fd_metadata = shm_open(METADATA_MEMORY, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

        ftruncate(fd_metadata, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)));

    }

    //int *metada_ptr = mmap(NULL, sizeof(sem_t) + (METADATA_SIZE * sizeof(metadata_node_t)), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

}

metadata_node_t *create_metadata_node(int total_pixeles, int width, int heigth) {

    metadata_node_t *node = malloc(sizeof(metadata_node_t));

    node.total_pixeles = total_pixeles;
    node.width = width;
    node.heigth = heigth;
    node.has_decoder = 1;

    sem_t semaphore;

    sem_init(&semaphore, 1, 0);

    node.image_semaphore = semaphore;

    return node;

}

int add_metadata_node(metadata_node_t *metada_ptr, metadata_node_t *node) {

    for(int i = 0; i < METADATA_SIZE; i++) {

        if(metada_ptr[i] == NULL) {

            memcopy(metada_ptr[i], node, sizeof(metadata_node_t));

            free(node);

            return i;

        }

    }

}

















int main() {

    printf("Main");

    return 0;

}