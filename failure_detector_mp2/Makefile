CC=g++

CFLAGS=-c -Wall -std=c++11

.PHONY: all clean

all: thread

thread: main.o thread.o
	$(CC) main.o thread.o -o thread.out

main.o: main.cpp thread.o
	$(CC) $(CFLAGS) main.cpp

thread.o: thread.cpp thread.h
	$(CC) $(CFLAGS) thread.cpp

# server: server.o utility.o 
# 	$(CC) server.o utility.o -o server.out

# client: client.o utility.o
# 	$(CC) client.o utility.o -o client.out

# test: test.o utility.o 
# 	$(CC) test.o utility.o -o test.out

# client.o: client.cpp utility.o
# 	$(CC) $(CFLAGS) client.cpp

# server.o: server.cpp utility.o
# 	$(CC) $(CFLAGS) server.cpp

# test.o: test.cpp utility.o
# 	$(CC) $(CFLAGS) test.cpp

# utility.o: utility.cpp utility.h
# 	$(CC) $(CFLAGS) utility.cpp 

clean:
	rm -f *.o *.out *.log