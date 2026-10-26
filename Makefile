CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -lpthread

torneo: main.c
	$(CC) $(CFLAGS) -o torneo main.c

clean:
	rm -f torneo