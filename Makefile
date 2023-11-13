# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/


# Targets
.PHONY: all
all: $(BIN)/download 

$(BIN)/download: $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)


.PHONY: clean
clean:
	rm -f $(BIN)/download