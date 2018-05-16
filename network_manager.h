#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H



#include "rpc/client.h"
#include "rpc/server.h"
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
#include <exception>
#include <memory>

using std::string;
using std::vector;


#define SLEEP_INTERVAL 2
#define RPC_TIMEOUT 3000

#define IP_LENGTH 30
#define PORT_LENGTH 6
// class Network_RPC:public Network{
// 	void connect();
// 	void run_server(string dest_ip,unsigned short port);
// };
struct Peer_struct{
	int id;
	char peerip[IP_LENGTH];
	char peerport[PORT_LENGTH];
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
struct Work_item{
	msg_t type;
	char source_ip[IP_LENGTH];
	char source_port[PORT_LENGTH];
	char dest_ip[IP_LENGTH];
	char dest_port[PORT_LENGTH];
	char additional_ip[IP_LENGTH];
	char additional_port[PORT_LENGTH];
	Work_item(){
		type=msg_t::UNKNOWN;
	}
	Work_item(msg_t	type_,const char* dest_ip_,const char* dest_port_,const char* source_ip_,const char* source_port_,const char* additional_ip_,const char* additional_port_){
			type=type_;
			strcpy(dest_ip,dest_ip_);
			strcpy(dest_port,dest_port_);
			strcpy(source_ip,source_ip_);
			strcpy(source_port,source_port_);
			strcpy(additional_ip,additional_ip_);
			strcpy(additional_port,additional_port_);
	}
	Work_item& operator=(const Work_item& rhs){
		if(this!=&rhs){
			type=rhs.type;
			strcpy(source_ip,rhs.source_ip);
			strcpy(source_port,rhs.source_port);
			strcpy(dest_ip,rhs.dest_ip);
			strcpy(dest_port,rhs.dest_port);
			strcpy(additional_ip,rhs.additional_ip);
			strcpy(additional_port,rhs.additional_port);
		}
		return *this;
	}

	void print(){
		cout<<"type"<<type-'a'<<"item: source:"<<source_ip<<" "<<source_port
		<<" dest:"<<dest_ip<<" "<<dest_port
		<<" "<<additional_ip<<" "<<additional_port<<endl;
	}
};


class Network{
public:
	Network();
	virtual void connect()=0;
	virtual void disconnect()=0;
protected:
	char _hostname[IP_LENGTH];
	char _port[PORT_LENGTH];
};

class Network_UDP:public Network{
public:
	Network_UDP(const char* hostname,const char* port);
	static bool send_message(msg_t type,const char* dest,const char* dest_port,const char* source,const char* source_port,const char* info,const char* info_port);
	msg_t recv_message(char* source, char* source_port, char* info, char* info_ip);
	void wait_message_from_peers(vector<Peer_struct>& input);
	void connect();
	void disconnect();
private:
	int _sockfd=-1;
};

class Network_RPC:public Network{
public:
	
	Network_RPC(const char* hostname,const char* port);
	void connect();
	void disconnect();
	
	template <typename Arg>
	void bind(string fname,Arg args){
		if(_rpc_server)
		_rpc_server->bind(fname,args);
	}
	template <typename... Arg>
	static RPCLIB_MSGPACK::object_handle call_rpc(string ip_addr,unsigned short port,string function_name,Arg... args){

		rpc::client client(ip_addr,port);
		client.set_timeout(RPC_TIMEOUT);
		auto handle = client.call(function_name,std::forward<Arg>(args)...);
		return handle;
	}
	std::unique_ptr<rpc::server> _rpc_server;
};


#endif
