#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <semaphore.h>
#include "constants.h"

//Chunk
typedef struct Node Node_t;
typedef struct Node{
	Node_t* next;
	int value;
	int index;
	int metadata_id;
	bool dirtyBit;
} Node_t;

typedef struct ImageChunk
{
	int size;
	Node_t* head;
	Node_t* tail;
    sem_t sem_encoders;
} ImageChunk_t;


//Metadata
typedef struct MetadataEntry
{
    int total_pixeles;
    int width, height;
    int has_decoder;
    sem_t image_semaphore;
} MetadataEntry_t;

typedef struct
{
    MetadataEntry_t metadata_array[MAX_METADATA_NODES];
    sem_t sem_metadata;
}shared_struct_metadata;

// encoder
struct Descriptor
{
    uint16_t encrypted_px;
    int struct_index;
    int px_position;
    int is_read;
    char insertion_time[9];
};
struct Descriptor desc_array[2500];

struct ImgData
{
    int width, height, color_channels;
    unsigned char *img_ptr;
    size_t img_size;
};


#endif // TYPES_H