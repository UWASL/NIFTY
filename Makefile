CC=g++
CFLAGS=-c -Wall -std=c++17 -lpthread -pthread
DEPS = dv.h

SRC_DIR := src
OBJ_DIR := obj

.PHONY: all clean hermes partitioner

all: hermes partitioner

debug: CFLAGS += -DDEBUG -g
debug: all

hermes: $(OBJ_DIR)/daemon.o $(OBJ_DIR)/dv.o
	$(CC) -o $@ $^ -pthread

partitioner: $(OBJ_DIR)/partitioner.o
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/dv.h  | $(OBJ_DIR) 
	$(CC) -o $@ $< $(CFLAGS)

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm  -r $(OBJ_DIR) hermes partitioner
