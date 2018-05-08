#include "network_manager.h"
#include "logger.h"
#include "config.h"
#include "membership.h"

#include <thread>
#include <vector>
#include <iostream>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using std::vector;


	Membership m;
bool running=true;
void my_handler(int s){
		running=false;
       m.stop();

}

int main(int argc,char** argv){
	if(argc!=2){
		printf("usage:./service id\n");
		exit(0);
	}
	int id=atoi(argv[1]);
	initialize_log(id);
	Config* conf=get_config();
	conf->id=id;

	//signal for control c
	struct sigaction sigIntHandler;

   sigIntHandler.sa_handler = my_handler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;

   sigaction(SIGINT, &sigIntHandler, NULL);


	m.start();
	while(running){
		
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