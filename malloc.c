#include "Heap.h"


//
// Allocate a chunk of memory on the heap
//
void* malloc(size_t size) {

	//Figure out the size alignment (should be 8 bytes)
	if (size == 0) {return NULL;}
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



		//Okay, we are at the end of the heap, and there is no space left.
		// So try to allocate more space on to the end of the heap
		//
		// Note: If this block is not free, then create a new block in the space
		size_t toAdd;
		if (IS_FREE_BLOCK(block)) {
			toAdd = size-block->size;
		} else {
			toAdd = size+BLOCK_LENGTH;
		}
	
		size_t added = increase_heap(toAdd);
		if (!added) {return (unlock_heap(), NULL); /* Alloc Error */ }


		//Do I need to create a new block, or resize the current block
		if (IS_FREE_BLOCK(block)) {
			//Resize the old block
			block->size += added;
			block->checksum = block_checksum(block);

		} else {
			//Create a new block
			block = create_block(block,NULL,block->size,added-BLOCK_LENGTH);
		}
	}

	//Figure out if I need to split this current block?
	if (block->size > (size+BLOCK_LENGTH)) {
		split_block(block,size);
	}
	
	//Make the size negative to indicate that this block is in use
	block->size *= -1;
	block->checksum = block_checksum(block);

	unlock_heap();
	return (void*) (block+1);
}
