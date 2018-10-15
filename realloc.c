#include "Heap.h"
#include <string.h>		/* For memcpy */


//
// Reallocate the size of something stored in the heap
//
void* my_realloc(void* ptr, size_t size) {

	if (!ptr) {return my_malloc(size);}

	lock_heap();

	pHeap_Block_t block = PTR_BLOCK(ptr);
	if (!valid_block) {return (unlock_heap(), NULL); /* HEAP Corruption */ }	

	//Figure out the size alignment (should be 8 bytes)
	if (size == 0) {return NULL;}
	size = NEXT_HEAP_ALIGN(size);


	//See if the realloc is smaller (We might need to split it!)
	//	Figure out if it is worth the space to split it
	if ((size + BLOCK_LENGTH) < BLOCK_SIZE(block)) {
		split_block(block,size);
		return (unlock_heap(), ptr);
	}


	//See if this block is already large enough
	if (BLOCK_SIZE(block) >= size) {return (unlock_heap(), ptr);}

	//See if I can merge with possible free blocks in front of me
	pHeap_Block_t end_block = block;
	size_t total_size = BLOCK_SIZE(block);
	while((end_block->next) && (total_size < size)) {
		if (!valid_block(end_block->next)) {return (unlock_heap(), NULL); /* Heap Corruption */}
		if (!IS_FREE_BLOCK(end_block->next)) {break;}
		end_block = end_block->next;
		total_size += (BLOCK_LENGTH + end_block->size);
	}

	//Yes, merge those blocks
	if (total_size >= size) {
		//Sew up the linked-list
		block->next = end_block->next;
		if (end_block->next) {end_block->next->pre = block;}

		//Resize the array
		block->size = -total_size;

		//Update the checksums
		block->checksum = block_checksum(block);
		if (end_block->next) {end_block->next->checksum = block_checksum(end_block->next);}

		//The combined block might be too big
		//	Figure out if it is worth the space to split it
		if ((size + BLOCK_LENGTH) < BLOCK_SIZE(block)) {
			split_block(block,size);
		}

		return (unlock_heap(), ptr);
	}



	//No, we need to completely reallocate the buffer
	unlock_heap();
	void* new_buffer = my_malloc(size);
	if (!new_buffer) {return NULL;}

	//Copy the data into the new buffer
	memcpy(new_buffer,ptr,BLOCK_SIZE(block));
	
	//Release the old buffer
	my_free(ptr);
	return new_buffer;	
}
