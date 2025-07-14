CC = gcc
CFLAGS = -std=c17 -g -Wall -Wextra -Werror -O2
LDFLAGS = -lm

SRC_DIR = ../src
TEST_DIR = .

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

SRC_OBJS = $(SRC_FILES:.c=.o)
TEST_OBJS = $(TEST_FILES:.c=.o)

.PHONY: all clean

all: test_batch

test_batch: $(SRC_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_OBJS) $(TEST_OBJS) test_batch
