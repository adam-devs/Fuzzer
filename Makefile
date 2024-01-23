CC = g++
CFLAGS = -Wall -Wextra --std=c++17

fuzzer: fuzzer.cpp
	$(CC) $(CFLAGS) -o fuzzer fuzzer.cpp

clean:
	rm -f fuzzer
