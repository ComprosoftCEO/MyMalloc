#include <stdio.h>
#include <stdlib.h>
#include "Heap.h"

int main(void) {
	
	printf("Testing Heap Data");
	void* ptr = malloc(5);
	//printf("%p\n",ptr);

	dump_heap();

	free(ptr);

	return 0;
}
