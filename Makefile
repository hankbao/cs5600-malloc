CC = g++
CXXFLAGS = -Wall -std=c++14 -g

all: malloc

clean:
	rm -f malloc *.o

malloc: main.o allocator_base.o allocator_best.o allocator_worst.o allocator_first.o allocator_next.o
	$(CC) $(CXXFLAGS) -o malloc main.o allocator_base.o allocator_best.o allocator_worst.o allocator_first.o allocator_next.o

main.o: main.cc allocator_best.h
	$(CC) $(CXXFLAGS) -c main.cc

allocator_base.o: allocator_base.cc allocator_base.h chunk.h
	$(CC) $(CXXFLAGS) -c allocator_base.cc

allocator_best.o: allocator_best.cc allocator_best.h chunk.h
	$(CC) $(CXXFLAGS) -c allocator_best.cc

allocator_worst.o: allocator_worst.cc allocator_worst.h chunk.h
	$(CC) $(CXXFLAGS) -c allocator_worst.cc

allocator_first.o: allocator_first.cc allocator_first.h chunk.h
	$(CC) $(CXXFLAGS) -c allocator_first.cc

allocator_next.o: allocator_next.cc allocator_next.h chunk.h
	$(CC) $(CXXFLAGS) -c allocator_next.cc
