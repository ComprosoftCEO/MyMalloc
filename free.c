#include "Heap.h"


//
// Release a chunk of memory from the heap
//
void free(void* ptr) {

	//Test for nullptr
	if (!ptr) {return;}

	pHeap_Block_t block = PTR_BLOCK(ptr);
	if (!valid_block(block)) {return;}

	


}
