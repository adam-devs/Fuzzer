CC = g++
CFLAGS = -Wall -Wextra --std=c++17

all: fuzzer

fuzzer: fuzzer.o generate.o
	$(CC) $(CFLAGS) -o fuzzer fuzzer.o generate.o

generate.o: generate.hpp
	$(CC) $(CFLAGS) -c generate.cpp

fuzzer.o: fuzzer.cpp
	$(CC) $(CFLAGS) -c fuzzer.cpp

clean:
	rm -f *.o fuzzer
