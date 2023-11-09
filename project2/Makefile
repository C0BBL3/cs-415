CC = gcc
CFLAGS = -g -W -Wall

DEPS = string_parser.c string_parser.h MCP.h
PARTS = part1 part2 part3 part4
EXECUTABLES = $(PARTS)

all: $(EXECUTABLES)

$(PARTS): %: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(EXECUTABLES)