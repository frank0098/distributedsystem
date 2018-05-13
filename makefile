CC= clang++

CFLAGS = -c -std=c++14


.PHONY: all clean

all: server

server: network_manager.o server.o logger.o membership.o config.o file_server.o file_manager.o
	$(CC) -pthread network_manager.o server.o logger.o config.o membership.o file_server.o file_manager.o librpc.a -o server.out

server.o:
	$(CC) $(CFLAGS)  -I./  server.cpp

file_manager.o:
	$(CC) $(CFLAGS) -I./  file_manager.cpp

membership.o: 
	$(CC) $(CFLAGS)  -I./ membership.cpp

network_manager.o:
	$(CC) $(CFLAGS) -I./ network_manager.cpp 

file_server.o:
	$(CC) $(CFLAGS)  -I./ file_server.cpp

logger.o:
	$(CC) $(CFLAGS) logger.cpp 

config.o:
	$(CC) $(CFLAGS) config.cpp

clean:
	rm -f *.o *.out
