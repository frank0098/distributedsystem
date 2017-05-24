#include "detector.h"

detector::detector(std::list<string> *mem, std::list<string> *am,loggerThread *lg):_members(mem),_alive_members(am),_logger(lg){
	_logger->add_write_log_task("detector start");
	_nw=new network_udp(DETECTORPORT);
	_nw->connect();
}
detector::~detector(){
	_nw->disconnect();
	delete _nw;
}	
void* detector::run(){

	while(1){
		for(auto m:*_members){
			char source[INET6_ADDRSTRLEN];
			network_udp::send_msg(msg_t::PING,SERVERPORT,m.c_str());
			_logger->add_write_log_task("Detector: SEND PING TO "+m);
			_nw->recv_msg(source);
			_logger->add_write_log_task("Detector: recv msg from "+string(source));

		}
		getchar();
	}
}