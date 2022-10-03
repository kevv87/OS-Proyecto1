#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <semaphore.h>

typedef struct Node Node_t;
typedef struct Node{
	Node_t* next;
	int value;
	int index;
	int metadata_id;
	bool dirtyBit;
} Node_t;

typedef struct ImageChunk{
	int size;
	Node_t* head;
	Node_t* tail;
    sem_t write_semaphore;
} ImageChunk_t;

#endif // TYPES_H