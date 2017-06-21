#ifndef DETECTOR_H
#define DETECTOR_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <list>
#include <string>
#include <algorithm>

#define DETECTOR_SLEEP_TIME_CONFIG 5

class detector:public Thread{
public:
	detector(std::list<string> *mem, alive_member *am, loggerThread *lg);
	virtual ~detector();
	void* run();
private:

	std::list<std::string> *_members;
	alive_member *_am;
	loggerThread* _lg;
	network_udp *_nw;
};

class detector_sender:public Thread{
public:
	detector_sender(std::list<string> *mem,alive_member *am,loggerThread *lg);
	virtual ~detector_sender();
	void* run();
private:
	std::list<std::string> *_members;
	alive_member *_am;
	loggerThread* _lg;
	// network_udp *_nw;
};

enum detector_state{
	START_PHASE=0,
	PING_ACK_PHASE=1,
	SUSPICIOUS=2
};

extern detector_state ds;
extern std::vector<std::string> suspicious_dead_members;

#endif