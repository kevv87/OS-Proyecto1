#ifndef IMAGE_CHUNK
#define IMAGE_CHUNK

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/types.h"
#include "include/shared_memory.h"

ImageChunk_t create_image_chunk(int node_quantity);
ImageChunk_t shm_create_image_chunk(void *ptr_to_shm_start, int node_quantity);

void append_item(ImageChunk_t *chunk, Node_t *pixelToAppend);
void shm_append_item(void *ptr_to_shm_start, Node_t *nodeToAppend);

int append_n_default_nodes(ImageChunk_t *chunk, int node_quantity);
int shm_append_n_default_nodes(void * ptr_to_shm_start, int node_quantity);

int replace_nth_pixel(ImageChunk_t *imageChunk, Node_t *newPixel, int pos);

Node_t *get_pixel_by_index(ImageChunk_t *imageChunk, int indexToRetrieve);
Node_t *get_pixel_by_metadata_id(ImageChunk_t *imageChunk, int metadataId);

Node_t *navigate_to_prev_node(ImageChunk_t *imageChunk, int nextIndex);
Node_t *navigate_to_node(ImageChunk_t *imageChunk, int indexToRetrieve);


#endif // IMAGE_CHUNK