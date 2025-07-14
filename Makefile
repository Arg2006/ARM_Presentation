# Makefile for Part 4

CC = gcc

# Assembler flags and sources
CFLAGS = -std=c17 -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -Wall -Wextra
SRC = extension.c $(wildcard sensors/*.c) $(wildcard src/*.c)
OUT = extension

.PHONY: all extension clean

all: extension

extension: $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) -lwiringPi -lm

clean:
	rm -f $(OUT)
