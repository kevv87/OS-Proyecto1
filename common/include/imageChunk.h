#ifndef IMAGE_CHUNK
#define IMAGE_CHUNK

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "shared_memory.h"

ImageChunk_t create_image_chunk(void *ptr_to_shm_start, int node_quantity);
void append_item(void *ptr_to_shm_start, Node_t *nodeToAppend);
int append_n_default_nodes(void * ptr_to_shm_start, int node_quantity);

Node_t *navigate_to_prev_node(ImageChunk_t *imageChunk, int nextIndex);


#endif // IMAGE_CHUNK