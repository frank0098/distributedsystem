
#include <string>
#include <iostream>
#include <arpa/inet.h>

#include "thread.h"
#include "network.h"
#include "logger.h"
#include <sys/socket.h>


class service:public Thread{
public:
	service(loggerThread *lg,network_udp* svnw);
	~service();
	void* run();
private:
	loggerThread* _lg;
	network_udp* _nw;
	network_udp* _server_nw;
};