CC = g++
CFLAGS = -Wall -Wextra -fsanitize=address -g --std=c++17

SRC_DIR := src
OBJ_DIR := obj
$(shell mkdir -p $(OBJ_DIR))


all: fuzz-sat
fuzz-sat: $(OBJ_DIR)/fuzzer.o $(OBJ_DIR)/generate.o $(OBJ_DIR)/generate_sat.o $(OBJ_DIR)/mutate.o $(OBJ_DIR)/coverage.o $(OBJ_DIR)/process_output.o
	$(CC) $(CFLAGS) -o fuzz-sat $(OBJ_DIR)/fuzzer.o $(OBJ_DIR)/generate.o $(OBJ_DIR)/generate_sat.o $(OBJ_DIR)/mutate.o $(OBJ_DIR)/coverage.o $(OBJ_DIR)/gcov.o $(OBJ_DIR)/process_output.o

$(OBJ_DIR)/generate.o: $(SRC_DIR)/generate.cpp $(SRC_DIR)/generate.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/generate.cpp -o $(OBJ_DIR)/generate.o

$(OBJ_DIR)/generate_sat.o: $(SRC_DIR)/generate_sat.cpp $(SRC_DIR)/generate_sat.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/generate_sat.cpp -o $(OBJ_DIR)/generate_sat.o

$(OBJ_DIR)/mutate.o: $(SRC_DIR)/mutate.cpp $(SRC_DIR)/mutate.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/mutate.cpp -o $(OBJ_DIR)/mutate.o

$(OBJ_DIR)/process_output.o: $(SRC_DIR)/process_output.cpp $(SRC_DIR)/process_output.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/process_output.cpp -o $(OBJ_DIR)/process_output.o

$(OBJ_DIR)/coverage.o: $(SRC_DIR)/coverage.cpp $(SRC_DIR)/coverage.hpp $(OBJ_DIR)/gcov.o
	$(CC) $(CFLAGS) -c $(SRC_DIR)/coverage.cpp -o $(OBJ_DIR)/coverage.o

$(OBJ_DIR)/gcov.o: $(SRC_DIR)/gcov.cpp $(SRC_DIR)/gcov.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/gcov.cpp -o $(OBJ_DIR)/gcov.o

$(OBJ_DIR)/fuzzer.o: $(SRC_DIR)/fuzzer.cpp $(SRC_DIR)/fuzzer.hpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/fuzzer.cpp -o $(OBJ_DIR)/fuzzer.o

clean:
	rm -f fuzz-sat
	rm -rf $(OBJ_DIR)

