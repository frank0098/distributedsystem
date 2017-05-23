#ifndef NETWORK_H
#define NETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
enum msg_t {
	UNKNOWN=-1;
	JOIN=0,
	EXIT=1,
	PING=2,
	ACK=3,
	INDIRECT_PING=4,
	INDIRECT_ACK=5
};
#define PORT 7000

class network_server:public network{
public:
	void connect() override;
	static void server_send(int sockfd,msg_t msgtype);
};

class network_client:public network{
public:
	void connect() override;
};


class network{
public:
	network(string hostname);
	virtual void connect()=0;
	bool send_msg(msg_t type);
	msg_t recv_msg();
	int get_fd();
private:
	bool connected;
	int _sockfd;
	string _hostname;
};



#endif