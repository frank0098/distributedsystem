CC=g++

CFLAGS=-c -Wall -std=c++11

OBJECTS= main.o membership.o server.o detector.o network.o logger.o thread.o member.o service.o workQueue.o detector_sender.o election_listener.o election.o

REMOTECONTROLLER_OBJECTS= remotecontroller.o network.o

.PHONY: all clean

all: dfs remotecontroller

remotecontroller: remotecontroller.o
	$(CC) $(REMOTECONTROLLER_OBJECTS) -o remotecontroller.out

dfs:$(OBJECTS)
	$(CC) $(OBJECTS) $(DEBUG) -pthread -o dfs.out

remotecontroller.o: remotecontroller.cpp network.h
	$(CC) $(CFLAGS) remotecontroller.cpp

main.o: main.cpp membership.h
	$(CC) $(CFLAGS) main.cpp

membership.o: membership.cpp membership.h server.h detector.h network.h logger.h thread.h member.h thread.h service.h workQueue.h
	$(CC) $(CFLAGS) membership.cpp

election.o: election.cpp election.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) election.cpp

election_listener.o: election_listener.cpp election.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) election_listener.cpp

detector.o: detector.cpp detector.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) detector.cpp

detector_sender.o: detector_sender.cpp detector.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) detector_sender.cpp

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