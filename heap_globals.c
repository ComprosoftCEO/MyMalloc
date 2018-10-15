#include "Heap.h"
#include <pthread.h>	/* For thread functions */
#include <unistd.h>		/* For sbrk */


//Global Heap Variables
static pthread_mutex_t heap_lock = PTHREAD_MUTEX_INITIALIZER;
static pHeap_Block_t heap_start = 0;



//
// Initialize the heap variables (do this once)
//
static bool init_heap(void) {

	//Start of the heap
	heap_start = (pHeap_Block_t) malloc(HEAP_START_SIZE + BLOCK_LENGTH); //sbrk(0);
	if (!heap_start) {
	//if (!IS_VALID_SBRK(heap_start)) {
		heap_start = 0;
		return false;
	}

	//Increase the heap size to the starting size
	//	Size = START_SIZE + BLOCK_LENGTH
	//if (!IS_VALID_SBRK(
	//		sbrk(HEAP_START_SIZE + BLOCK_LENGTH)
	//	)) {return false;}

	//Configure values inside starting block
	heap_start->pre = heap_start->next = NULL;
	heap_start->size = HEAP_START_SIZE;
	heap_start->checksum = block_checksum(heap_start);
	return true;
}



//
// Lock the heap and return a pointer to do heap operations
//
pHeap_Block_t lock_heap(void) {

	//Lock the heap
	pthread_mutex_lock(&heap_lock);

	//Allocate the heap default stuff once
	if (!heap_start) {
		if (!init_heap()) {
			pthread_mutex_unlock(&heap_lock);
			return NULL;
		}
	}

	return heap_start;
}



//
// Unlock the heap
//
void unlock_heap(void) {
	pthread_mutex_unlock(&heap_lock);
}




//
// Validate a given block
//
bool valid_block(pHeap_Block_t block) {
	if (!block) {return false;}
	if (block->checksum != block_checksum(block)) {return false;}
	return true;
}


//
// Add more bytes to the end of the heap (return bytes added)
//
size_t increase_heap(size_t bytes_needed) {

	//Round up bytes_needed to the next heap increment
	bytes_needed = NEXT_HEAP_INC(bytes_needed);

	void* new_ptr = realloc(heap_start,bytes_needed);
	if (!new_ptr) {return 0;}
	
	//if (!IS_VALID_SBRK(sbrk(bytes_needed))) {return 0;}
	return bytes_needed;
}



//
// Create a new block in the heap
//	Assumes that prev, next, offset and size are legit
// 
pHeap_Block_t create_block(pHeap_Block_t prev, pHeap_Block_t next, size_t offset, size_t size) {
	
	if (!prev) {prev = heap_start;}

	//Configure the linked-list
	pHeap_Block_t new_block = NEXT_BLOCK(prev,offset);
	new_block->pre = prev;
	new_block->next = next;
	prev->next = new_block;
	if (next) {next->pre = new_block;}

	//Update the size
	new_block->size = size;

	//Recompute checksums
	if (prev) {prev->checksum = block_checksum(prev);}
	if (next) {next->checksum = block_checksum(next);}
	new_block->checksum = block_checksum(new_block);
	return new_block;
}



//
// Split one free block into two free blocks 
//	Assumes new_size < block->size, and everything is aligned
//
void split_block(pHeap_Block_t block, size_t new_size) {
	create_block(block, block->next, new_size, BLOCK_SIZE(block) - (new_size + BLOCK_LENGTH));

	//Test for a negative size
	if (block->size < 0) {block->size = -new_size;}
	else {block->size = new_size;}
	block->checksum = block_checksum(block);
}
