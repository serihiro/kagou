PROGRAM_NAME = kagou
OBJS = main.o
CC = gcc
CFLAGS = -g -Wall -Wextra -O2

main: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM_NAME) $(OBJS)

main.o: src/main.c
	$(CC) -c src/main.c

clean:
	$(RM) kagou
