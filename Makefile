CC = g++
CFLAGS = -Wall -Wextra -fsanitize=address -g --std=c++17

OBJ_DIR := obj
$(shell mkdir -p $(OBJ_DIR))


all: fuzzer
fuzzer: fuzzer.o generate.o generate_sat.o mutate.o coverage.o process_output.o
	$(CC) $(CFLAGS) -o fuzzer $(OBJ_DIR)/fuzzer.o $(OBJ_DIR)/generate.o $(OBJ_DIR)/generate_sat.o $(OBJ_DIR)/mutate.o $(OBJ_DIR)/coverage.o $(OBJ_DIR)/gcov.o $(OBJ_DIR)/process_output.o

generate.o: src/generate.cpp
	$(CC) $(CFLAGS) -c src/generate.cpp -o $(OBJ_DIR)/generate.o

generate_sat.o: src/generate_sat.cpp
	$(CC) $(CFLAGS) -c src/generate_sat.cpp -o $(OBJ_DIR)/generate_sat.o

mutate.o: src/mutate.cpp
	$(CC) $(CFLAGS) -c src/mutate.cpp -o $(OBJ_DIR)/mutate.o

process_output.o: src/process_output.cpp
	$(CC) $(CFLAGS) -c src/process_output.cpp -o $(OBJ_DIR)/process_output.o

coverage.o: src/coverage.cpp gcov.o
	$(CC) $(CFLAGS) -c src/coverage.cpp -o $(OBJ_DIR)/coverage.o

gcov.o: src/gcov.cpp
	$(CC) $(CFLAGS) -c src/gcov.cpp -o $(OBJ_DIR)/gcov.o

fuzzer.o: src/fuzzer.cpp
	$(CC) $(CFLAGS) -c src/fuzzer.cpp -o $(OBJ_DIR)/fuzzer.o

clean:
	rm -f fuzzer
	rm -rf $(OBJ_DIR)

