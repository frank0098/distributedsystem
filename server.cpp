#include "network_manager.h"
#include <thread>

int main(){
	Network* nw=new Network_Multicast();
	auto f=std::thread([&](){
		nw->run_server();
	});
	while(true){
		sleep(2);
		nw->multicast();
	}

}