#include "detector.h"

detector::detector(std::list<string> *mem, std::list<string> *am,loggerThread *lg):_members(mem),_alive_members(am),_logger(lg){

}
detector::~detector(){

}
void* detector::run(){

	for(auto m:*_members){
		network_client *nw=new network_client(m);
		nw->connect();
		if(nw->is_connected()){
			_nw.push_back(nw);
		}
		else{
			delete nw;
		}
	}
	while(1){
		for(auto nw:_nw){
			nw->send_msg(msg_t::PING);
			_logger->add_write_log_task("SEND PING TO "+nw->hostname());
			std::cout<<"send ping to "+nw->hostname()<<endl;
		}
		sleep(2);
	}
}