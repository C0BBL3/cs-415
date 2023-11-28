CC = gcc
CFLAGS = -g -W -Wall

DEPS = string_parser.c string_parser.h account.h
PARTS = part1
EXECUTABLES = $(PARTS)

all: $(EXECUTABLES)

$(PARTS): %: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(EXECUTABLES)