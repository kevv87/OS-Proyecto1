#include "include/imageChunk.h"

ImageChunk_t create_image_chunk(void *ptr_to_shm_start, int node_quantity){
	ImageChunk_t chunk = (ImageChunk_t) {
		.size = 0,
		.head = NULL,
		.tail = NULL
	};
    ImageChunk_t * imageChunk_ptr = (ImageChunk_t *) ptr_to_shm_start;
    *imageChunk_ptr = chunk;
    
	append_n_default_nodes(ptr_to_shm_start, node_quantity);

	return chunk;
}

int append_n_default_nodes(void * ptr_to_shm_start, int node_quantity){
    for(int i=0; i < node_quantity; i++){
        Node_t pixel = (Node_t) {
            .next = NULL,
            .value = 80+i,
            .index = -1,
            .metadata_id = i+3,
            .dirtyBit = false
        };
        append_item(ptr_to_shm_start, &pixel);
    }
    return 0;
}

Node_t * determine_next_available_shm_ptr(void *ptr_to_shm_start, ImageChunk_t *chunk){
    void * avail_ptr = ptr_to_shm_start;
    avail_ptr += sizeof(ImageChunk_t) + chunk->size * sizeof(Node_t);
    return (Node_t *) avail_ptr;
}

void append_item(void *ptr_to_shm_start, Node_t *nodeToAppend){
    
    ImageChunk_t *chunk = (ImageChunk_t *) ptr_to_shm_start;

    Node_t* newNode = 
        determine_next_available_shm_ptr(ptr_to_shm_start, chunk);
    
    nodeToAppend->index = chunk->size;

    write_shared_memory(newNode, nodeToAppend, sizeof(Node_t));

	if(chunk->size == 0){
		chunk->head = newNode;
		chunk->tail = newNode;
		newNode->next = newNode;
	}else{
        Node_t* tempNode;

		tempNode = chunk->tail;
		chunk->tail = newNode;
		tempNode->next = chunk->tail;
		newNode->next = chunk->head;
	}

	chunk->size++;

	return;
}

int replace_nth_pixel(ImageChunk_t *imageChunk, Node_t *newPixel, int pos){

	if(!imageChunk)
		return -1;

	Node_t *prevNode = navigate_to_prev_node(imageChunk, pos);
    Node_t *tempNode = malloc(sizeof(Node_t));

	if(!prevNode)
		return -1;

    tempNode = read_shared_memory(tempNode, prevNode->next, sizeof(Node_t));
	Node_t *newNode = 
        write_shared_memory(prevNode->next, newPixel, sizeof(Node_t));
 
	newNode->next = tempNode->next;
    newNode->index = tempNode->index;

	if(pos==0){
		imageChunk->head = newNode;
		imageChunk->head->index = 0;
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

Node_t *get_pixel_by_index(ImageChunk_t *imageChunk, int indexToRetrieve){
	Node_t *pixelResult;
	Node_t *node_retrieved = navigate_to_node(imageChunk, indexToRetrieve);

	if (node_retrieved)
		return node_retrieved;

	return NULL;
}

Node_t *get_pixel_by_metadata_id(ImageChunk_t *imageChunk, int metadataId){
	Node_t *node_i = imageChunk->head;
	for(int i = 0; i<imageChunk->size;i++){
		if(node_i->metadata_id == metadataId){
			return node_i;
		}
		node_i = node_i->next;
	}
	printf("Not found\n");
	return NULL;
}