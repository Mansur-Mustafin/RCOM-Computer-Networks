# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/

URL = ftp://ftp.up.pt/pub/kodi/timestamp.txt  # We can change this URL.
# ftp://rcom:rcom@netlab1.fe.up.pt
# ftp://rcom:rcom@netlab1.fe.up.pt/some-url-path
# ftp://netlab1.fe.up.pt
# ftp://ftp.up.pt/pub/kodi/timestamp.txt


# Targets
.PHONY: all
all: $(BIN)/download

$(BIN)/download: main.c $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)

.PHONY: run
run: $(BIN)/download
	./$(BIN)/download $(URL)

.PHONY: clean
clean:
	rm -f $(BIN)/download