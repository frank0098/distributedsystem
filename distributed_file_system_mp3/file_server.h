#ifndef FILESERVER_H
#define FILESERVER_H
#include "thread.h"
#include "network.h"
#include "logger.h"
#include "member.h"
#include <arpa/inet.h>
#include <unordered_map>
#include <cstdlib> 
#include <ctime> 
#include <sstream>
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/mman.h>

#define DUPLICATE_COUNT 3
const char* map_in_file="filescnmcnmcnm";
class file_server:public Thread{
public:
	file_server(loggerThread *lg,alive_member *am);
	virtual ~file_server();
	void* run();
private:
	loggerThread* _lg;
	network_server* _nw;
	alive_member *_am;
	// std::unordered_map<std::string,std::vector<string> > file_addr_map;


};
#endif