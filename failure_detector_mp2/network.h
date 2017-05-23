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
	JOIN=0,
	EXIT=1,
	PING=2,
	ACK=3,
	INDIRECT_PING=4,
	INDIRECT_ACK=5
};
#define SERVER_PORT 7000
#define DETECTOR_PORT 7001
class network{
public:
	network()=default;
	void get_connection();
	void send_msg(msg_t type);
	msg_t recv_msg();
private:
	int _sockfd;
	string _hostname;
};



#endif