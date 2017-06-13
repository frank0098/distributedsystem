#ifndef ELECTION_H
#define ELECTION_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <arpa/inet.h>


class election:public Thread{
public:
	election(loggerThread *lg);
	~election();
	void* run();
private:
	loggerThread* _lg;

};

class election_listener:public Thread{
public:
	election_listener(loggerThread *lg);
	~election_listener();
	void* run();
private:
	loggerThread* _lg;
	network_udp* _nw;

};
#endif