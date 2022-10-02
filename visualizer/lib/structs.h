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

typedef struct Statistic {
    int total_used_memory;
    double total_semaphore_waited_time;
    int total_data_transfered;
    double total_kernel_time;
    int total_pixels_175;

    int total_instances;
    int chunk_size;
    int metadata_size;
    int statistics_size;
    sem_t semaphore_visualizer;
} Statistic_t;
