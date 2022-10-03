#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <semaphore.h>

#define MAX_METADATA_NODES 15

//Chunk
typedef struct Node Node_t;
typedef struct Node{
	Node_t* next;
	int value;
	int index;
	int metadata_id;
	bool dirtyBit;
    int px_position;
    char insertion_time[9];
} Node_t;

typedef struct ImageChunk
{
	int size;
	Node_t* head;
	Node_t* tail;
    sem_t sem_encoders;
} ImageChunk_t;

// Metadata
typedef struct Metadata_Node {
    int total_pixeles;
    int width;
    int height;
    int has_decoder;
    // int index // indice que comparte el encoder y el decoder
    sem_t image_semaphore;
} Metadata_Node_t;

typedef struct Metadata_Table {
    Metadata_Node_t metadata_array[MAX_METADATA_NODES];
    sem_t sem_metadata_table;
} Metadata_Table_t;

// Statistic
typedef struct {
    int total_used_memory;
    double total_semaphore_waited_time;
    int total_data_transfered;
    double total_kernel_time;
    int total_pixels_175;
    int total_instances;
    int chunk_size;
    sem_t semaphore_statistic;
    sem_t semaphore_visualizer;
} Statistic_t;












typedef struct ImgData
{
    int width, height, color_channels;
    unsigned char *img_ptr;
    size_t img_size; // todo ************************************* preguntar a Chino
} ImgData_t;


#endif // TYPES_H