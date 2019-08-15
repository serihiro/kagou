PROGRAM_NAME = kagou
OBJS = main.o request_handler.o util.o request.o response.o
CC = gcc
CFLAGS = -g -O2 -Wall -Wextra
SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)

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

response.o: src/response.c
		$(CC) $(CFLAGS) -c src/response.c

.PNONY :format
format:
	clang-format $(SRCS) $(HEADERS) -i

clean:
	$(RM) kagou
	$(RM) ./*.o
