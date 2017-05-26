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
	UNKNOWN=0,
	JOIN=1,
	EXIT=2,
	PING=3,
	ACK=4,
	INDIRECT_PING=5,
	INDIRECT_ACK=6,
	FAIL=7
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
	network_udp(const char* port,bool stm);
	void connect();
	static bool send_msg(msg_t msgtype,const char* port,const char* ip_addr);
	msg_t recv_msg(char* ip_addr);
	void recv_msg(char* msg,size_t msg_size,char* ip_addr);
	static bool send_msg(const char* msg,size_t msg_size,const char* port,const char* ip_addr);
private:
	const char* _PORT;
	bool _settimeout;

};


extern void *get_in_addr(struct sockaddr *sa);
using std::cout;
using std::endl;



#endif