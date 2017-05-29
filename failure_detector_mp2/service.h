
#include <string>
#include <iostream>
#include <arpa/inet.h>

#include "thread.h"
#include "network.h"
#include "logger.h"


class service:public Thread{
public:
	service(loggerThread *lg);
	~service();
	void* run();
private:
	loggerThread* _lg;
	network_udp* _nw;
};