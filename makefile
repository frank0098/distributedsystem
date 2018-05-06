CC=clang++

CFLAGS = -c -std=c++11


.PHONY: all clean

all: server

server: network_manager.o server.o logger.o membership.o config.o
	$(CC) -pthread network_manager.o server.o logger.o config.o membership.o -o server.out



server.o:
	$(CC) $(CFLAGS) server.cpp

membership.o: 
	$(CC) $(CFLAGS) membership.cpp

network_manager.o:
	$(CC) $(CFLAGS) network_manager.cpp 


logger.o:
	$(CC) $(CFLAGS) logger.cpp 

config.o:
	$(CC) $(CFLAGS) config.cpp

clean:
	rm -f *.o *.out
