CC = gcc
CFLAGS = -g -W -Wall
DEPS = hellomake.h
OBJ = hellomake.o hellofunc.o
NAME = hellomake

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

%.o: %.c $(DEPS)
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f $(NAME) *.o