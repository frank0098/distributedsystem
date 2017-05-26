CC=g++

CFLAGS=-c -Wall -std=c++11

OBJECTS= main.o membership.o server.o dissemination.o detector.o network.o logger.o thread.o member.o

.PHONY: all clean

all: failure_detector 

failure_detector:$(OBJECTS)
	$(CC) $(OBJECTS) $(DEBUG) -o -pthread failure_detector.out

main.o: main.cpp membership.h
	$(CC) $(CFLAGS) main.cpp

membership.o: membership.cpp membership.h server.h dissemination.h detector.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) membership.cpp

detector.o: detector.cpp detector.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) detector.cpp

dissemination.o: dissemination.cpp dissemination.h network.h logger.h
	$(CC) $(CFLAGS) dissemination.cpp

server.o: server.cpp server.h network.h logger.h thread.h member.h
	$(CC) $(CFLAGS) $(DEBUG) server.cpp

network.o: network.cpp network.h
	$(CC) $(CFLAGS) network.cpp

logger.o: logger.cpp logger.h workQueue.h
	$(CC) $(CFLAGS) logger.cpp

thread.o: thread.cpp thread.h
	$(CC) $(CFLAGS) thread.cpp

member.o: member.cpp member.h
	$(CC) $(CFLAGS) member.cpp

clean:
	rm -f *.o *.out *.log