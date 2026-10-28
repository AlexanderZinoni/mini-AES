all= main

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

INC = include
SRC = src

main: main.o table.o mini-AES.o
	$(CC) $(CFLAGS) main.o table.o mini-AES.o -o main

table.o: $(SRC)/table.c $(INC)/table.h
	$(CC) $(CFLAGS) -c $< -o $@

mini-AES.o: $(SRC)/mini-AES.c $(INC)/mini-AES.h $(INC)/table.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: $(SRC)/main.c $(INC)/table.h $(INC)/mini-AES.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o main