#ifndef ELECTION_H
#define ELECTION_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <arpa/inet.h>


class election:public Thread{
public:
	election(loggerThread *lg,alive_member *am);
	~election();
	void* run();
private:
	loggerThread* _lg;
	alive_member *_am;

};

class election_listener:public Thread{
public:
	election_listener(loggerThread *lg,alive_member *am);
	~election_listener();
	void* run();
private:
	loggerThread* _lg;
	network_udp* _nw;
	alive_member *_am;

};
#endif