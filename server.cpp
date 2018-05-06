#include "network_manager.h"
#include "logger.h"
#include "config.h"
#include "membership.h"

#include <thread>
#include <vector>
#include <iostream>
using std::vector;

int main(int argc,char** argv){
	if(argc!=2){
		printf("usage:./service id\n");
		exit(0);
	}
	int id=atoi(argv[1]);
	initialize_log(id);
	Config* conf=get_config();
	conf->id=id;
	Membership m;
	m.start();
	while(true){
		
	}


	// Network_UDP* nw=new Network_UDP("127.0.0.1","3456");
	// nw->connect();

	// Network_UDP* nw2=new Network_UDP("127.0.0.1","3457");
	// nw2->connect();
	// auto f=std::thread([&](){
	// 	char source[30];
	// char info[30];
	// char source_port[30];
	// char info_port[30];
	// 	while(true){
	// 		msg_t ms=nw2->recv_message(source,source_port,info,info_port);
	// 		std::cout<<"recvmessage"<<ms<<std::endl;
	// 		msg_t type=msg_t::JOIN;
	// 		Network_UDP::send_message(type,"127.0.0.1","3457","127.0.0.1","3457","127.0.0.1","3457");



	// 	}
	// });
	// vector<Peer_struct> input;
	// while(true){
	// 	std::cout<<"loop1"<<std::endl;
	// 	sleep(1);
	// 	msg_t type=msg_t::JOIN;
	// 	Network_UDP::send_message(type,"127.0.0.1","3456","127.0.0.1","3456","127.0.0.1","3456");
	// 	nw->wait_message_from_peers(input);
	// }

}