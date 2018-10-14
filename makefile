CC=gcc
CFLAGS=
LIBS=-lpthread

PROGRAM=test.out

OBJS=\
	heap_globals.o \
	checksum.o \
	malloc.o


all: $(PROGRAM)

%.out: %.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c Heap.h
	$(CC) -c $(CFLAGS) -o $@ $<



# Remove all compiled files
.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGRAM)

# Run the program
.PHONY: run
run:
	./$(PROGRAM)