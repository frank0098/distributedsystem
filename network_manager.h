#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H


#include "logger.h"

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using std::string;
using std::vector;
// class Network_RPC:public Network{
// 	void connect();
// 	void run_server(string dest_ip,unsigned short port);
// };
struct Peer_struct{
	string peerip;
	string peerport;
	bool alive=false;
	bool suspicious=false;
};

enum msg_t {
	UNKNOWN='a',
	TIMEOUT='b',
	JOIN='c',
	EXIT='d',
	PING='e',
	ACK='f',
	INDIRECT_PING='g',
	INDIRECT_ACK='h'
};

class Network{
public:
	Network();
	virtual void connect()=0;
	virtual void disconnect()=0;
protected:
	int _sockfd=-1;
	string _hostname;
	unsigned short _port;
};

class Network_UDP:public Network{
public:
	Network_UDP(string hostname,unsigned short port);
	static bool send_message(msg_t type,const char* dest,const char* dest_port,const char* source,const char* source_port,const char* info,const char* info_port);
	msg_t recv_message(char* source, char* source_port, char* info, char* info_ip);
	void wait_message_from_peers(vector<Peer_struct>& input);
	void connect();
	void disconnect();
private:
};


#endif
