CC := gcc
SRC := $(shell find -name "*.c")
BIN := main
INCLUDE := -Iinclude
FLAGS := -Wall -Wextra

COMM := $(CC) $(INCLUDE) $(SRC) -o $(BIN) $(FLAGS)

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	$(COMM)

clean:
	rm -f $(BIN)