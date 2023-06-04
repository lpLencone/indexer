CC = gcc
SRC = $(shell find -name "*.c")
BIN = main
INCLUDE = -Iinclude
FLAGS = -Wall -Wextra

COMM = $(CC) $(INCLUDE) $(SRC) -o $(BIN) $(FLAGS)

all:
	$(COMM)
