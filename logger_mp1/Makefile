CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -Wall -std=c++11
.PHONY: all clean

all: server client

server: server.o utility.o 
	$(CC) server.o utility.o -o server.out

client: client.o utility.o
	$(CC) client.o utility.o -o client.out

client.o: client.cpp utility.o
	$(CC) $(CFLAGS) client.cpp

server.o: server.cpp utility.o
	$(CC) $(CFLAGS) server.cpp

utility.o: utility.cpp utility.h
	$(CC) $(CFLAGS) utility.cpp 

clean:
	rm -f *.o *.out