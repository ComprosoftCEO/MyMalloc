#include "Heap.h"


//
// Release a chunk of memory from the heap
//
void free(void* ptr) {

	//Test for nullptr
	if (!ptr) {return;}

	lock_heap();

	//Make sure the block is valid and not already freed
	pHeap_Block_t block = PTR_BLOCK(ptr);
	if (!valid_block(block)) {return unlock_heap(); /*  Bad Pointer */}
	if (IS_FREE_BLOCK(block)) {return unlock_heap(); /* Double Free! */ }

	//Indiate that this block is now free
	block->size *= -1;
	block->checksum = block_checksum(block);

	//Merge blocks on the front and end of this block
	//	Start by finding the most previous free block
	pHeap_Block_t start_merge = block;
	while(start_merge->pre) {
		if (!valid_block(start_merge->pre)) {return unlock_heap(); /* HEAP CORRUPTION */}
		if (!IS_FREE_BLOCK(start_merge->pre)) {break;}
		start_merge = start_merge->pre;
	}

	//Now find out the total number of consecutive free blocks, and the total lenght
	pHeap_Block_t end_merge = start_merge;
	size_t total_size = start_merge->size;
	while (end_merge->next) {
		if (!valid_block(end_merge->next)) {return unlock_heap(); /* HEAP CORRUPTION */}
		if (!IS_FREE_BLOCK(end_merge->next)) {break;}
		end_merge = end_merge->next;
		total_size += (BLOCK_LENGTH + end_merge->size);
	}


	//Reconfigure the linked-list
	start_merge->next = end_merge->next;
	if (end_merge->next) {end_merge->next->pre = start_merge;}
	start_merge->size = total_size;
	
	unlock_heap();
}
