#ifndef FILESERVER_H
#define FILESERVER_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <arpa/inet.h>
class file_server:public Thread{
public:
	file_server(loggerThread *lg,alive_member *am);
	~file_server();
	void* run();
private:
	loggerThread* _lg;
	network_server* _nw;
	alive_member *_am;


};
#endif