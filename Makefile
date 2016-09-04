PROGRAM_NAME = kagou
OBJS = main.o request_handler.o
CC = gcc
CFLAGS = -g -Wall -Wextra -O2

main: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM_NAME) $(OBJS)

main.o: src/main.c
	$(CC) -c src/main.c

request_handler.o: src/request_handler.c
	$(CC) -c src/request_handler.c

clean:
	$(RM) kagou
