#include <stdbool.h>

typedef struct Pixel {
	int valor;
	int index;
	int metadata_id;
	bool dirtyBit;
} Pixel_t;

typedef struct Node Node_t;
typedef struct Node{
	Pixel_t* pixel;
	Node_t* next;
	int index;	
} Node_t;

typedef struct ImageChunk{
	int size;
	Node_t* head;
	Node_t* tail;
} ImageChunk_t;