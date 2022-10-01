#include <semaphore.h>

typedef struct Pixel {
    int value;
    int index;
    int metadata_id;
    int dirty_bit;
} Pixel_t;

typedef struct Node Node_t;

typedef struct Node {
    Pixel_t* pixel;
    Node_t* next;
    int index;
} Node_t;

typedef struct Stat {
    int total_used_memory;
    int total_semaphore_wait;
    int total_data_transfered;
    int total_kernel_time;
    int total_pixels_175;
    int total_instances;
    sem_t semaphore_visualizer
} Stat_t;
