#include <stdio.h>
#include <stdlib.h>

int main(void) {
	void* ptr = malloc(5);
	printf("%p\n",ptr);

	return 0;
}
