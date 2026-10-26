CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -lpthread

torneo: main.c tablero.c
	$(CC) $(CFLAGS) -o torneo main.c tablero.c

clean:
	rm -f torneo