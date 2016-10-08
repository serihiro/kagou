PROGRAM_NAME = kagou
OBJS = main.o request_handler.o util.o request.o
CC = gcc
CFLAGS = -g -O0 -Wall -Wextra

main: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM_NAME) $(OBJS)

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

request_handler.o: src/request_handler.c
	$(CC) $(CFLAGS) -c src/request_handler.c

util.o: src/util.c
	$(CC) $(CFLAGS) -c src/util.c

request.o: src/request.c
	$(CC) $(CFLAGS) -c src/request.c

clean:
	$(RM) kagou
	$(RM) ./*.o
