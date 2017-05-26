CC=g++

CFLAGS=-c -Wall -std=c++11

.PHONY: all clean

all: failure_detector

failure_detector:main.o membership.o server.o dissemination.o detector.o network.o logger.o thread.o member.o
	$(CC) main.o membership.o server.o dissemination.o detector.o network.o logger.o thread.o member.o -o failure_detector.out

main.o: main.cpp membership.o
	$(CC) $(CFLAGS) main.cpp

membership.o: membership.cpp membership.h server.o dissemination.o detector.o network.o logger.o thread.o member.o
	$(CC) $(CFLAGS) membership.cpp

detector.o: detector.cpp detector.h network.o logger.o thread.o member.o
	$(CC) $(CFLAGS) detector.cpp

dissemination.o: dissemination.cpp dissemination.h network.o logger.o
	$(CC) $(CFLAGS) dissemination.cpp

server.o: server.cpp server.h network.o logger.o thread.o member.o
	$(CC) $(CFLAGS) server.cpp

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