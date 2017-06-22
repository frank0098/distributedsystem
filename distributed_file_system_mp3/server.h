#ifndef SERVER_H
#define SERVER_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"

#include <arpa/inet.h>
#include <vector>
#include <string>
#include <iostream>

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
	// std::unordered_map<std::string,std::vector<string> > *file_addr_map;



};
#endif