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
    Pixel_t *pixel;
    for(int i=0; i < node_quantity; i++){
        pixel = malloc(sizeof(Pixel_t));
        *pixel = (Pixel_t) {
            .value = 80+i,
            .index = -1,
            .metadata_id = i+3,
            .dirtyBit = false
        };
        append_item(chunk, pixel);
    }
    return 0;
}

void append_item(ImageChunk_t *chunk, Pixel_t *pixelToAppend){
	Node_t* newNode = malloc(sizeof(Node_t));
	*newNode = (Node_t) {
		.pixel = pixelToAppend,
		.next = NULL,
		.index = chunk->size
	};

	Node_t* tempNode;

	if(chunk->size == 0){
		chunk->head = newNode;
		chunk->tail = newNode;
		newNode->next = newNode;
	}else{
		tempNode = chunk->tail;
		chunk->tail = newNode;
		tempNode->next = chunk->tail;
		newNode->next = chunk->head;
	}

	chunk->size++;

	pixelToAppend->index = newNode->index;

	return;
}

int replace_nth_pixel(ImageChunk_t *imageChunk, Pixel_t *newPixel, int pos){

	if(!imageChunk)
		return -1;

	Node_t *prevNode = navigate_to_prev_node(imageChunk, pos);

	if(!prevNode)
		return -1;

	Node_t *newNode = malloc(sizeof(Node_t));
	Node_t *tempNode = prevNode->next;
	prevNode->next =  newNode;
	newNode->next = tempNode->next;
	newNode->pixel = newPixel;
	newNode->pixel->index = tempNode->index;

	if(pos==0){
		imageChunk->head = newNode;
		imageChunk->head->pixel->index = 0;
	}
	
	free(tempNode);
	return 0;
}

Node_t *navigate_to_prev_node(ImageChunk_t *imageChunk, int nextIndex){
	if (nextIndex >= imageChunk->size)
		return NULL;
	
	if(nextIndex == 0)
		return imageChunk->tail;

	Node_t *node_i = imageChunk->head;
	for(int i = 0; i<imageChunk->size;i++){
		if(i == nextIndex-1)
			return node_i;
		
		node_i = node_i->next;
	}

	return NULL;
}

Node_t *navigate_to_node(ImageChunk_t *imageChunk, int indexToRetrieve){
	Node_t* node_prev = navigate_to_prev_node(imageChunk, indexToRetrieve);

	if(node_prev)
		return node_prev->next;

	return NULL;
}

Pixel_t *get_pixel_by_index(ImageChunk_t *imageChunk, int indexToRetrieve){
	Pixel_t *pixelResult;
	Node_t *node_retrieved = navigate_to_node(imageChunk, indexToRetrieve);

	if (node_retrieved)
		return node_retrieved->pixel;

	return NULL;
}

Pixel_t *get_pixel_by_metadata_id(ImageChunk_t *imageChunk, int metadataId){
	Node_t *node_i = imageChunk->head;
	for(int i = 0; i<imageChunk->size;i++){
		if(node_i->pixel->metadata_id == metadataId){
			printf("Found! %d\n", node_i->pixel->metadata_id);
			return node_i->pixel;
		}
		node_i = node_i->next;
	}
	printf("Not found\n");
	return NULL;
}