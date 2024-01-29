CC = g++
CFLAGS = -Wall -Wextra --std=c++17

all: fuzzer
fuzzer: fuzzer.o generate.o generate_sat.o 
	$(CC) $(CFLAGS) -o fuzzer fuzzer.o generate.o generate_sat.o

generate.o: generate.cpp
	$(CC) $(CFLAGS) -c generate.cpp

generate_sat.o: generate_sat.cpp
	$(CC) $(CFLAGS) -c generate_sat.cpp

coverage.o: coverage.cpp
	$(CC) $(CFLAGS) -c coverage.cpp

fuzzer.o: fuzzer.cpp
	$(CC) $(CFLAGS) -c fuzzer.cpp

clean:
	rm -f *.o fuzzer
