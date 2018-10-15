#ifndef MY_MALLOC_HEADER
#define MY_MALLOC_HEADER

#include <stdint.h>		/* For intptr_t and uintptr_t */
#include <stdlib.h>		/* for malloc, free, realloc, calloc */
#include <stdbool.h>


//Define the heap metadata structure
typedef struct Heap_Block {
	struct Heap_Block* pre;		// Previous block in the list
	struct Heap_Block* next;	// Next block in the list
	intptr_t size;				// Number of bytes in the block. If negative, then the block is in use
	uintptr_t checksum;			// Make sure the block values are valid
} Heap_Block_t, *pHeap_Block_t;



#define BLOCK_LENGTH  (sizeof(Heap_Block_t))		// 16 or 32 bytes in a block


//Heap parameters
#define HEAP_ALIGN_SHIFT (3)						// Bits to shift the heap alignment
#define HEAP_ALIGN       (1 << HEAP_ALIGN_SHIFT)	// All variables should be aligned to 8 bytes
#define HEAP_START_SIZE  (1024)						// Default size for the heap
#define HEAP_INC_SHIFT   (10)						// Bits to shift for heap size increment
#define HEAP_INC_SIZE    (1 << HEAP_INC_SHIFT)		// Amount to incrament the heap


//Round X up to the next binary increment
#define __ROUND_UP(x,shift) ((((x) >> shift) + (((x) & ((1<<shift)-1)) != 0)) << shift)
#define NEXT_HEAP_INC(x)    __ROUND_UP(x,HEAP_INC_SHIFT)
#define NEXT_HEAP_ALIGN(x)  __ROUND_UP(x,HEAP_ALIGN_SHIFT)


//Block Macros (only use with a raw pointer)
#define PTR_BLOCK(x)       (((pHeap_Block_t) (x)) - 1)		// Get the block for a void* pointer
#define BLOCK_SIZE(x)      (((x)->size < 0) ? ((x)->size*(-1)) : ((x)->size))	// Number of bytes in the block
#define IS_FREE_BLOCK(x)   ((x)->size > 0)					// Test if a block is free
#define IS_VALID_SBRK(x)   ((x) != ((void*) -1))			// Test if the pointer returned from sbrk is valid
#define NEXT_BLOCK(x,size) ((pHeap_Block_t) (((uint8_t*) ((x)+1)) + size))


#ifdef __cplusplus
extern "C" {
#endif


//Use a mutex to guarantee thread safety
pHeap_Block_t lock_heap(void);
void unlock_heap(void);


//Only call these methods AFTER the heap is locked by the current thread
bool valid_block(pHeap_Block_t block);
uintptr_t block_checksum(pHeap_Block_t block);
size_t increase_heap(size_t bytes_needed);
pHeap_Block_t create_block(pHeap_Block_t prev, pHeap_Block_t next, size_t offset, size_t size);
void split_block(pHeap_Block_t block, size_t new_size);

//Debugging functions
void dump_heap(void);
void print_heap_block(pHeap_Block_t block);
void print_heap_entry(void* ptr);

#ifdef __cplusplus
}
#endif

#endif
