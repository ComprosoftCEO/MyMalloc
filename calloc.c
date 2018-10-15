#include "Heap.h"
#include <string.h>		/* For memset */


//
// Allocate a piece of memory, and set everything to 0's
//
void* calloc(size_t num, size_t size) {
	void* ptr = malloc(num*size);
	if (ptr) {memset(ptr,0,num*size);}
	return ptr;
}
