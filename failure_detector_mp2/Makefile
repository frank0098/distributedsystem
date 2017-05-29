CC=g++

CFLAGS=-c -Wall -std=c++11

OBJECTS= main.o membership.o server.o detector.o network.o logger.o thread.o member.o service.o workQueue.o

REMOTECONTROLLER_OBJECTS= remotecontroller.o network.o 
.PHONY: all clean

all: failure_detector remotecontroller

remotecontroller: remotecontroller.o
	$(CC) $(REMOTECONTROLLER_OBJECTS) -o remotecontroller.out

failure_detector:$(OBJECTS)
	$(CC) $(OBJECTS) $(DEBUG) -pthread -o failure_detector.out

remotecontroller.o: remotecontroller.cpp network.h
	$(CC) $(CFLAGS) remotecontroller.cpp

main.o: main.cpp membership.h
	$(CC) $(CFLAGS) main.cpp

membership.o: membership.cpp membership.h server.h detector.h network.h logger.h thread.h member.h thread.h service.h workQueue.h
	$(CC) $(CFLAGS) membership.cpp

detector.o: detector.cpp detector.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) detector.cpp

server.o: server.cpp server.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) server.cpp

service.o: service.cpp network.h logger.h thread.h
	$(CC) $(CFLAGS) service.cpp

network.o: network.cpp network.h
	$(CC) $(CFLAGS) network.cpp

logger.o: logger.cpp logger.h workQueue.h
	$(CC) $(CFLAGS) logger.cpp

workQueue.o: workQueue.cpp workQueue.h thread.h
	$(CC) $(CFLAGS) workQueue.cpp

thread.o: thread.cpp thread.h
	$(CC) $(CFLAGS) -pthread thread.cpp

member.o: member.cpp member.h
	$(CC) $(CFLAGS) member.cpp

clean:
	rm -f *.o *.out *.log