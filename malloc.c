#include "Heap.h"
#include <unistd.h>			/* For sbrk */

void* malloc(size_t size) {

	//Figure out the size alignment (should be 8 bytes)
	size = NEXT_HEAP_ALIGN(size);

	//Get the starting free block of the heap
	pHeap_Block_t block = lock_heap();
	if (!block) {return NULL;}
	if (!valid_block(block)) {return (unlock_heap(), NULL);}

	//Traverse the linked-list until a free block with enough space is discovered
	while(block->size < size) {

		//Make sure that this block is valid (to prevent heap corruption)
		if (!valid_block(block)) {
			//HEAP IS CORRUPTED!!!!
			return (unlock_heap(), NULL);
		}

		// Go to the next block (if it exists)
		if (block->next) {block = block->next; continue;}

		//Okay, we are at the end of the list
		// So try to allocate more space on to the end of the heap
		//
		// Note: If this block is not free, then allocate a new block in the space
		if (IS_FREE_BLOCK(block)) {
			if (!increase_heap(block,size - block->size)) {
				return (unlock_heap(), NULL);
			}
		} else {
			
		}

	}

	//Figure out if I need to split this current block?
	if (block->size > (size+BLOCK_LENGTH)) {
		

	}


}
