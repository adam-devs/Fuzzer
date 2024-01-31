CC = g++
CFLAGS = -Wall -Wextra -fsanitize=address -g --std=c++17

all: fuzzer
fuzzer: fuzzer.o generate.o generate_sat.o mutate.o coverage.o process_output.o
	$(CC) $(CFLAGS) -o fuzzer fuzzer.o generate.o generate_sat.o mutate.o coverage.o gcov.o process_output.o

generate.o: generate.cpp
	$(CC) $(CFLAGS) -c generate.cpp

generate_sat.o: generate_sat.cpp
	$(CC) $(CFLAGS) -c generate_sat.cpp

mutate.o: mutate.cpp
	$(CC) $(CFLAGS) -c mutate.cpp

process_output.o: process_output.cpp
	$(CC) $(CFLAGS) -c process_output.cpp

coverage.o: coverage.cpp gcov.o
	$(CC) $(CFLAGS) -c coverage.cpp

gcov.o: gcov.cpp
	$(CC) $(CFLAGS) -c gcov.cpp

fuzzer.o: fuzzer.cpp
	$(CC) $(CFLAGS) -c fuzzer.cpp

clean:
	rm -f *.o fuzzer

