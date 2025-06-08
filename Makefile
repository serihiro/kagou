PROGRAM_NAME = kagou
CC = gcc
CFLAGS = -g -O2 -Wall -Wextra -fsanitize=address -I./src
SRCS = $(wildcard src/*.c)
LIB_SRCS = $(filter-out src/main.c,$(SRCS))
OBJS=$(SRCS:.c=.o)
LIB_OBJS=$(LIB_SRCS:.c=.o)
HEADERS = $(wildcard src/*.h)

main: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM_NAME) $(OBJS)

main.o: src/main.c request_handler.o util.o request.o response.o
	$(CC) $(CFLAGS) -c src/main.c

$(OBJS): $(HEADERS)

request_handler.o: util.o src/request_handler.c src/request_handler.h
	$(CC) $(CFLAGS) -c src/request_handler.c

request.o: util.o src/request.c src/request.h
	$(CC) $(CFLAGS) -c src/request.c

response.o: util.o src/response.c src/response.h
		$(CC) $(CFLAGS) -c src/response.c

util.o: src/util.c src/util.h
	$(CC) $(CFLAGS) -c src/util.c

tests/test.o: tests/test.c $(HEADERS)
	$(CC) $(CFLAGS) -c tests/test.c -o tests/test.o

tests/run_tests: $(LIB_OBJS) tests/test.o
	$(CC) $(CFLAGS) -o tests/run_tests $(LIB_OBJS) tests/test.o

.PHONY: test
test: tests/run_tests
	./tests/run_tests

.PNONY :format
format:
	clang-format $(SRCS) $(HEADERS) -i

clean:
	$(RM) kagou
	$(RM) $(OBJS)
	$(RM) tests/run_tests tests/test.o
