#include "include/imageChunk.h"

ImageChunk_t create_image_chunk(int node_quantity){
	ImageChunk_t chunk = (ImageChunk_t) {
		.size = 0,
		.head = NULL,
		.tail = NULL
	};

	append_n_default_nodes(&chunk, node_quantity);

	return chunk;
}

int append_n_default_nodes(ImageChunk_t *chunk, int node_quantity){
    Pixel_t pixel;
    for(int i=0; i < node_quantity; i++){
        pixel = (Pixel_t) {
            .valor = 80+i,
            .index = -1,
            .metadata_id = i+3,
            .dirtyBit = false
        };
		append_item(chunk, &pixel);
    }
    return 0;
}

void append_item(ImageChunk_t *chunk, Pixel_t *pixelToAppend){
	Node_t* newNode = malloc(sizeof(Node_t));
	*newNode = (Node_t) {
		.pixel = pixelToAppend,
		.next = newNode,
		.prev = newNode,
		.index = chunk->size
	};

	if(chunk->size == 0){
		chunk->head = newNode;
		chunk->tail = newNode;
	}else{
		chunk->head->next = newNode;
		chunk->tail = newNode;
	}

	chunk->size++;

	pixelToAppend->index = newNode->index;

	return;
}