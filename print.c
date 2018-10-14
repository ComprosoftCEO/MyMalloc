#include "Heap.h"
#include <stdio.h>


//
// Dump out all of the entries in the heap
//
void dump_heap(void) {
	pHeap_Block_t block = lock_heap();
	if (!block) {printf("Lock Failed!\n"); return;}
	
	do {
		print_heap_entry(block+1);
	} while (block = block->next);
}


//
// Print out a block in the heap
//
void print_heap_block(pHeap_Block_t block) {
	
	if (!block) {printf("Invalid Block!\n"); return;}

	printf("%llx: Next: %llx\tPre: %llx\tSize: %lld\tChecksum:%llu",
		block, block->next, block->pre,
		(long long) block->size, (long long) block->checksum
	);

	if (!valid_block(block)) {printf("\t*INVALID*\n");}
	printf("\n");
}


//
// Print out the data returned from malloc, calloc, or realloc
//
void print_heap_entry(void* ptr) {

	if (!ptr) {printf("Invalid Pointer!\n");}

	pHeap_Block_t block = PTR_BLOCK(ptr);
	print_heap_block(block);

}
