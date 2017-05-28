#ifndef DETECTOR_H
#define DETECTOR_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <list>
#include <string>

#define DETECTOR_SLEEP_TIME_CONFIG 5

class detector:public Thread{
public:
	detector(std::list<string> *mem, alive_member *am, loggerThread *lg);
	~detector();
	void* run();
private:

	std::list<std::string> *_members;
	alive_member *_am;
	loggerThread* _logger;
	network_udp *_nw;
};
#endif