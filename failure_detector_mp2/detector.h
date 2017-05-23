#ifndef DETECTOR_H
#define DETECTOR_H
#include "thread.h"
#include "network.h"

class detector::public Thread{
public:

	void run();
private:
	void join();
	void quit();
	void send_ping();
	void send_indirect_ping();
};
#endif