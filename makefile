CC=clang++

CFLAGS = -c -std=c++11


.PHONY: all clean

all: server

server: network_manager.o server.o
	$(CC) network_manager.o server.o -o server.out

server.o: network_manager.o
	$(CC) $(CFLAGS) server.cpp

network_manager.o:
	$(CC) $(CFLAGS) network_manager.cpp 

clean:
	rm -f *.o *.out
