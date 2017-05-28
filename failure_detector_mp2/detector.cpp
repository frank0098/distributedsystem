#include "detector.h"

detector::detector(std::list<string> *mem, alive_member *am,loggerThread *lg):_members(mem),_am(am),_logger(lg){
	_logger->add_write_log_task("detector start");
	_nw=new network_udp(DETECTORPORT,true);
	_nw->connect();
}
detector::~detector(){
	_nw->disconnect();
	delete _nw;
	cout<<"END DETECTOR"<<endl;
}	
void* detector::run(){

	// JOIN
	// For all possible address send JOIN (including itself)
	// if recv ACK then add that into memberlist
	cout<<"start running"<<endl;
	char source[INET6_ADDRSTRLEN];
	for(auto m:*_members){
		if(!network_udp::send_msg(msg_t::JOIN,SERVERPORT,m.c_str())){
			_logger->add_write_log_task("Detector: FAIL TO CONNECT "+m);
			continue;
		}
		// _logger->add_write_log_task("Detector: SEND JOIN TO "+m);
		msg_t msgtype = _nw->recv_msg(source);
		// cout<<"detector recv "<<msgtype<<endl;
		if(msgtype==msg_t::ACK){
			_am->add(m);
			_logger->add_write_log_task("Detector: Add "+string(source)+" to membership list");
			string cm=_am->get_alive_member_list();
			// cout<<cm<<endl;
			_logger->add_write_log_task("Detector: current members: "+cm);
			
		}   
		else{
			_logger->add_write_log_task("Detector: === FATAL ERROR === recv unknown msg from "+m);
		}
		source[0]='\0';
	}

	// RUN
	// randomly select ONE machine to send PING
	// if recv ACK we good
	// else
		// send K machine INDIRECT_PING
			// if recv INDIRECT_ACK -we good
			// else
			// mark it failure. dissemination to ALL

	while(!pause_flag.is_true()){
		source[0]='\0';
		std::vector<string> alivemembers=_am->get_alive_member();
		// cout<<alivemembers.size()<<endl;
		for(auto m:alivemembers){
			// if(network_udp::send_msg(msg_t::PING,SERVERPORT,m.c_str()))
			{
				// _logger->add_write_log_task("FAIL TO CONNECT "+m);
				// continue;
			}
			network_udp::send_msg(msg_t::PING,SERVERPORT,m.c_str());
			
			#if DEBUG
			_logger->add_write_log_task("Detector: SEND PING TO "+m);
			#endif

			msg_t msgtype=_nw->recv_msg(source);
			// if timeout
			if(msgtype!=msg_t::ACK){
				_logger->add_write_log_task("Detector: "+m+" might have FAILED.SEND INDIRECT_PING");
				bool failflag=true;
				std::vector<std::string> other_machines=_am->ramdom_select_K(2);
				for(auto om:other_machines){
					network_udp::send_msg(msg_t::INDIRECT_PING,SERVERPORT,om.c_str());
					
					msg_t indirectmsgtype = _nw->recv_msg(source);
					if(msgtype==msg_t::INDIRECT_ACK){
						failflag=false;
						break;
					}
				}
				if(failflag){
					_am->remove(m);
					for(auto om:alivemembers){
						if(om!=m){
							network_udp::send_msg(msg_t::FAIL,SERVERPORT,om.c_str());
							_nw->recv_msg(source);
							char tmp[INET6_ADDRSTRLEN];
							strcpy(tmp,m.c_str());
							network_udp::send_msg(tmp,INET6_ADDRSTRLEN,SERVERPORT,om.c_str());
							_nw->recv_msg(source);
						}
					}
				}


			}
		}
		sleep(DETECTOR_SLEEP_TIME_CONFIG);
	}

	// EXIT
	// Send EXIT to all processes
	for(auto m:_am->get_alive_member()){
		source[0]='\0';
		network_udp::send_msg(msg_t::EXIT,SERVERPORT,m.c_str());
		_nw->recv_msg(source);
	}

}