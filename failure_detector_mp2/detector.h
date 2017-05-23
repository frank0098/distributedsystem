#ifndef DETECTOR_H
#define DETECTOR_H

class detector{
public:

	void run();
private:
	void join();
	void quit();
	void send_ping();
	void send_indirect_ping();
};
#endif