#include <semaphore.h>

typedef struct metadata_node {
    int total_pixeles;
    int width;
    int heigth;
    int has_decoder;
    sem_t image_semaphore;
} metadata_node_t;




