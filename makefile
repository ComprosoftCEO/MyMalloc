CC=gcc
CFLAGS=-g
LIBS=-lpthread

PROGRAM=test.out

OBJS=\
	heap_globals.o \
	checksum.o \
	print.o \
	malloc.o \
	calloc.o \
	realloc.o \
	free.o


all: $(PROGRAM)

%.out: %.cpp $(OBJS)
	g++ $(CFLAGS) -std=c++11 -o $@ $^ $(LIBS)

.PRECIOUS: %.o
%.o: %.c Heap.h
	$(CC) -c $(CFLAGS) -o $@ $<



# Remove all compiled files
.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGRAM)

# Run the program
.PHONY: run
run: $(PROGRAM)
	./$(PROGRAM)
