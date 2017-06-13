#ifndef SERVER_H
#define SERVER_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <arpa/inet.h>
class server:public Thread{
public:
	server(loggerThread *lg,alive_member *am);
	~server();
	void* run();
	network_udp* get_nw();
private:
	loggerThread* _lg;
	network_udp* _nw;
	alive_member *_am;


};
#endif