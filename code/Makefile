# Makefile to build the project
# NOTE: This file must not be changed.

# Parameters
CC = gcc
CFLAGS = -Wall

SRC = src/
INCLUDE = include/
BIN = bin/

# We can change this URL.
URL = ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt

# ftp://netlab1.fe.up.pt/pub.txt

# ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt
# ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg
# ftp://rcom:rcom@netlab1.fe.up.pt/files/pic2.png
# ftp://rcom:rcom@netlab1.fe.up.pt/files/crab.mp4

# ftp://ftp.up.pt/pub/kodi/timestamp.txt
# ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz
# ftp://ftp.up.pt/debian/ls-lR.gz

# ftp://demo:password@test.rebex.net/readme.txt

# ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin


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