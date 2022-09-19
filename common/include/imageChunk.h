#ifndef IMAGE_CHUNK
#define IMAGE_CHUNK

#include <stddef.h>
#include <stdlib.h>

#include "include/types.h"

ImageChunk_t create_image_chunk(int node_quantity);
void append_item(ImageChunk_t *chunk, Pixel_t *pixelToAppend);
int append_n_default_nodes(ImageChunk_t *chunk, int node_quantity);

#endif // IMAGE_CHUNK