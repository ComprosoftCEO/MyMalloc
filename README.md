# Malloc
My own implementation of malloc and free

<br>

## Background
I wrote this code for my CS-240 class at IUPUI for the project "Heap of Students". The project was all about using the C++ `new` and `delete` operators to store classes on the heap. For the blackbelt assignment, I decided to write my own implementation of malloc and free (which `new` and `delete` use internally) to show my understanding of how the heap works.

<br>

## Features
* Calls the function `sbrk()` to extend the data segment to automatically resize the heap
* Uses a pthread mutex to ensure thread-safe access
* Creates a checksum for each heap block to ensure data integrity
* Merges adjacent blocks when freed to prevent fragmentation

<br>

## Compiling and Running
The code is written for a Linux operating system, but can run on Windows using the [Windows Subsystem for Linux](https://docs.microsoft.com/en-us/windows/wsl/install-win10) (which is actually what I used to write and test the code). The provided makefile is used for building the [Testing Utility](#testing-utility) (_see below_), which was used to test the function calls during development. To use the code in other projects, merely include the [Code Files](#code-files) (_listed below_) in your project. Due to the way that linking works, these versions of `malloc()`, `calloc()`, `realloc()`, and `free()` will automatically replace the default versions in libc.

## Code Files
* __Heap.h__ - Contains the definition of a Heap Block structure, internal functions, debug functions, and other useful macros  
* __malloc.c, calloc.c, realloc.c, free.c__ - Function definitions for `malloc()`, `calloc()`, `realloc()`, and `free()` (as defined in _stdlib.h_)
* __checksum.c__ - Algorithm for computing the heap block checksum
* __heap_globals.c__ - Stores both global variables and functions used by all code files
* __print.c (optional)__ - Functions for printing out blocks in the heap (useful for debugging)

<br>

## The Algorithm
Every entry in the heap has a structure at `pointer-1` that stores information about the block
in the heap. The blocks are a doubly-linked list, and store:
* The previous block (pointer)
* The next block (pointer)
* The size of the block (in bytes)
	* _If the size is negative, then the block is inuse. Otherwise, the block is free._
* A checksum (for detecting heap corrution)

While the algorithm is well-commented in the code, here is the gist of what happens:
* After a call to malloc, the program searches for the first free block in the heap that
  has enough memory to store the requested size.
	* If no such block exists, then the heap is resized to fulfill the request
	* If the block is too big, then the program then calculates if it needs to split the
	  block into two smaller blocks
* When the program frees a block of memory, it looks to see if there are any contiguous
  free blocks in the front or behind, and merges the blocks together
* When the realloc method is called, the program looks for possible free blocks in front of the
  current block. If the free blocks are enough to resize the heap entry, then the blocks are combined
  together.
	* Otherwise, it allocates a new block using malloc, copies the data into the new block,
	  then frees the old block of data.

<br>

## Testing Utility
Although the code files can be used in any C or C++ project, the provided makefile is used to compile a testing utility (for debugging the heap). The debug utility (named _test.cpp_) works like a command-line interface. Commands are entered with the following format:
```
<Command> [Register] [Bytes]
```
Here is the list of all commands:
* __h__ - Show the help screen
* __q__ - Quit the program
* __m \<reg\> \<bytes\>__ - Malloc size \<bytes\> into \<reg\>
* __c \<reg\> \<bytes\>__ - Calloc size \<bytes\> into \<reg\>
* __r \<reg\> \<bytes\>__ - Realloc \<reg\> to size \<bytes\>
* __f \<reg\>__ - Free \<reg\> from memory
* __a \<reg\>__ - Print out the address of \<reg\>
* __p \<reg\>__ - Print out the chunk information of \<reg\>
* __d__ - Dump the contents of the heap
* __l__ - List all registers

<br>

## Notes
I wrote this implementation to prove that I could create my own version of malloc and free. However, for any practical project, the standard implementation of malloc and free is much safer and better-tested. I cannot guarantee that my version doesn't have any flaws or glitches.
