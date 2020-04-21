CC=gcc
FLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

proj2: proj2.c proj2.h
	$(CC) $(FLAGS) $^ -o proj2 -lpthread

.PHONY: clean
clean:
	rm -f proj2
