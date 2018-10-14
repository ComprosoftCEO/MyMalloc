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
	heap_start = (pHeap_Block_t) sbrk(0);
	if (!IS_VALID_SBRK(heap_start)) {
		heap_start = 0;
		return false;
	}

	//Increase the heap size to the starting size
	//	Size = START_SIZE + BLOCK_LENGTH
	if (!IS_VALID_SBRK(
			sbrk(HEAP_START_SIZE + BLOCK_LENGTH)
		)) {return false;}

	//Configure values inside starting block
	heap_start->pre = heap_start->next = NULL;
	heap_start->size = HEAP_START_SIZE;
	heap_start->magic = HEAP_MAGIC;
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
// Set the starting free block for the heap
//
void set_heap_start(pHeap_Block_t block) {
	//Test for NULL pointer
	if (block) {heap_start = block;}
}



//
// Validate a given block
//
bool valid_block(pHeap_Block_t block) {
	if (!block) {return false;}
	if (block->magic != HEAP_MAGIC) {return false;}
	if (block->checksum != block_checksum(block)) {return false;}
	return true;
}


//
// Add more bytes to the end of the heap
//
bool increase_heap(pHeap_Block_t last_block, size_t bytes_needed) {

	//Round up bytes_needed to the next heap increment
	size_t toAdd = NEXT_HEAP_INC(bytes_needed);
	if (!IS_VALID_SBRK(sbrk(toAdd))) {return false;}
	
	//Modify the size of the last heap block
	last_block->size += toAdd;
	last_block->checksum = block_checksum(block);
	return true;
}



//
// Split one free block into two free blocks
//
void split_block(pHeap_Block_t block, size_t new_size) {

	pHeap_Block_t new_block = (((uint8_t*) block+1) + new_size);

	//Sew up the linked list
	new_block->pre = block;
	new_block->next = block->next
	block->next = new_block;

	//Calculate the split block sizes
	new_block->size = (block->size - (new_size + BLOCK_LENGTH));
	new_block->magic = HEAP_MAGIC;
	new_block->checksum = block_checksum(new_block);
}
