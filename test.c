#include <stdio.h>
#include <stdlib.h>
#include "Heap.h"

int main(void) {
	
	printf("Testing Heap Data\n");
	
	void** ptrs = my_malloc(sizeof(void*) * 20);
	ptrs[0] = my_malloc(5);
	ptrs[1] = my_malloc(10);
	ptrs[2] = my_malloc(20);
	my_free(ptrs[0]);
	ptrs[0] = my_malloc(19);
	my_free(ptrs[1]);
	ptrs[1] = my_malloc(12);

	//void* ptr = my_malloc(5);
	//printf("%p\n",ptr);

	dump_heap();

	my_free(ptrs);

	return 0;
}
