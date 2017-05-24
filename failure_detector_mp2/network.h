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
#include <string>
#include <signal.h>

enum msg_t {
	UNKNOWN=-1,
	JOIN=0,
	EXIT=1,
	PING=2,
	ACK=3,
	INDIRECT_PING=4,
	INDIRECT_ACK=5
};
#define PORT "7000"
#define BACKLOG 10

class network{
public:
	network(std::string hostname);
	virtual void connect()=0;
	bool send_msg(msg_t type);
	msg_t recv_msg();
	int get_fd();
	bool is_connected();
	std::string hostname();
protected:
	bool _connected;
	std::string _hostname;
	int _sockfd;
};

class network_server:public network{
public:
	network_server();
	void connect() override;
	static bool server_send(int sockfd,msg_t msgtype);
};

class network_client:public network{
public:
	network_client(std::string hostname);
	void connect() override;
};

extern void *get_in_addr(struct sockaddr *sa);




#endif