#ifndef SERVER_H
#define SERVER_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include <arpa/inet.h>
class server:public Thread{
public:
	server(loggerThread *lg);
	~server();
	void* run();
private:
	loggerThread* _lg;
	network_udp* _nw;


};
#endif