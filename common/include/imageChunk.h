#ifndef IMAGE_CHUNK
#define IMAGE_CHUNK

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/types.h"

ImageChunk_t create_image_chunk(int node_quantity);
void append_item(ImageChunk_t *chunk, Pixel_t *pixelToAppend);
int append_n_default_nodes(ImageChunk_t *chunk, int node_quantity);
int replace_nth_pixel(ImageChunk_t *imageChunk, Pixel_t *newPixel, int pos);
Pixel_t *get_pixel_by_index(ImageChunk_t *imageChunk, int indexToRetrieve);
Node_t *navigate_to_prev_node(ImageChunk_t *imageChunk, int nextIndex);
Node_t *navigate_to_node(ImageChunk_t *imageChunk, int indexToRetrieve);
Pixel_t *get_pixel_by_metadata_id(ImageChunk_t *imageChunk, int metadataId);

#endif // IMAGE_CHUNK