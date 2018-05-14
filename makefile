CC= clang++

CFLAGS = -c -std=c++14


.PHONY: all clean

all: server dfs_client

dfs_client: network_manager.o dfs_client.o logger.o config.o file_manager.o file_server.o simple_json.o
	$(CC) network_manager.o dfs_client.o logger.o config.o file_manager.o file_server.o librpc.a simple_json.o -o client.out

server: network_manager.o server.o logger.o membership.o config.o file_server.o file_manager.o simple_json.o
	$(CC) -pthread network_manager.o server.o logger.o config.o membership.o file_server.o file_manager.o simple_json.o librpc.a -o server.out

dfs_client.o:
	$(CC) $(CFLAGS)  -I./  dfs_client.cpp

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

simple_json.o:
	$(CC) $(CFLAGS) simple_json.cpp

clean:
	rm -f *.o *.out
