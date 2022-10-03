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

ImageChunk_t * mapChunk(void * ptr_to_shm_start){

}

void append_item(void *ptr_to_shm_start, Node_t *nodeToAppend){
    
    ImageChunk_t *chunk = (ImageChunk_t *) ptr_to_shm_start;

    Node_t* newNode = 
        determine_next_available_shm_ptr(ptr_to_shm_start, chunk);
    printf("Puntero es:%d\n", newNode);
    
    nodeToAppend->index = chunk->size;

    //write_shared_memory(newNode, nodeToAppend, sizeof(Node_t));

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
    newNode->value = nodeToAppend->value;
    newNode->index = nodeToAppend->index;
    strcpy(newNode->insertion_time, nodeToAppend->insertion_time);
    newNode->metadata_id = nodeToAppend->metadata_id;
    newNode->dirtyBit = nodeToAppend->dirtyBit;
    newNode->px_position = nodeToAppend->px_position;

	chunk->size++;

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

