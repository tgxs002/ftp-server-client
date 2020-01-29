CC = gcc
CFLAGS =  -O -Wall -m32 -pthread
LIBS = -lm

all: server

server: server.c command_line.c verb_handler.c command_line.h verb_handler.h common.c common.h
	$(CC) $(CFLAGS) -o server server.c command_line.c verb_handler.c common.c

command_line: command_line.c command_line.h
	$(CC) $(CFLAGS) -o command_line.c

verb_handler: verb_handler.c verb_handler.h common.c common.h
	$(CC) $(CFLAGS) -o verb_handler.c common.c

clean:
	rm -f *.o server command_line verb_handler *~


