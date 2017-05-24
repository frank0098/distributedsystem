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
#define PORT "5000"
#define SERVERPORT "5001"
#define DETECTORPORT "5003"
#define BACKLOG 10

class network{
public:
	network(std::string hostname);
	virtual void connect()=0;
	bool send_msg(msg_t type);
	int get_fd();
	bool is_connected();
	std::string hostname();
	void disconnect();
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
	static msg_t recv_msg(int sockfd);
};

class network_client:public network{
public:
	network_client(std::string hostname);
	void connect() override;
	msg_t recv_msg();
};

class network_udp:public network{
public:
	network_udp(const char* port);
	void connect();
	static bool send_msg(msg_t msgtype,const char* port,const char* ip_addr);
	msg_t recv_msg(char* ip_addr);
private:
	const char* _PORT;

};


extern void *get_in_addr(struct sockaddr *sa);
using std::cout;
using std::endl;



#endif