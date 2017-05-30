#include "detector.h"

detector::detector(std::list<string> *mem, alive_member *am,loggerThread *lg):_members(mem),_am(am),_logger(lg){
	_logger->add_write_log_task("detector start");
	_nw=new network_udp(DETECTORPORT,true);
	_nw->connect();
}
detector::~detector(){
	_nw->disconnect();
	delete _nw;
	_logger->add_write_log_task("detector ends");
	// cout<<"END DETECTOR"<<endl;
}	
void* detector::run(){

	// JOIN
	// For all possible address send JOIN (including itself)
	// if recv ACK then add that into memberlist

	char source[INET6_ADDRSTRLEN];
	char msg_receive_buffer[BUFFER_SIZE];
	char additional_ip_received[INET6_ADDRSTRLEN];
	char msg_send_buffer[BUFFER_SIZE];
	while(true){

		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		pause_flag.lock();
		while(pause_flag.is_true()){
			pause_flag.cond_wait();
		}
		pause_flag.unlock();



		cout<<"start running"<<endl;
		for(auto m:*_members){
			network_udp::generate_msg(msg_send_buffer,msg_t::JOIN,m.c_str());
			if(!network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,m.c_str())){
				_logger->add_write_log_task("Detector: FAIL TO CONNECT "+m);
				continue;
			}
			// _logger->add_write_log_task("Detector: SEND JOIN TO "+m);
			// msg_t msgtype = _nw->recv_msg(source);
			// cout<<"detector recv "<<msgtype<<endl;
			_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source);
			msg_t msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
			if(msg_type==msg_t::JOIN_SUCCESS){

				if(_am->add(m)){
					_logger->add_write_log_task("Detector: Add "+string(source)+" to membership list");
					_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
				}
				else{
					_logger->add_write_log_task("Detector: "+string(source)+" already in the membership list");
				}
			}   
			else{
				_logger->add_write_log_task("Detector: === JOIN ERROR ===  recv msg from "+m);
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

		while(true){
			stop_flag.lock();
			if(stop_flag.is_true()){
				stop_flag.unlock();
				break;
			}
			stop_flag.unlock();
			pause_flag.lock();
			if(pause_flag.is_true()){
				pause_flag.unlock();
				break;
				// pause_flag.cond_wait();
			}
			pause_flag.unlock();

			source[0]='\0';
			std::vector<string> alivemembers=_am->get_alive_member();
			_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
			for(auto m:alivemembers){

				network_udp::generate_msg(msg_send_buffer,msg_t::PING,m.c_str());
				network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,m.c_str());
				// network_udp::send_msg(msg_t::PING,SERVERPORT,m.c_str());
				
				#if DEBUG
				_logger->add_write_log_task("Detector: SEND PING TO "+m);
				#endif

				_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source);
				msg_t msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				if(string(source)!=m) continue;
				// if timeout
				if(msg_type!=msg_t::ACK){
					_logger->add_write_log_task("Detector: receive " + to_string(msg_type)+" :"+m+" might have FAILED.SEND QUERY");
					bool failflag=true;
					std::vector<std::string> other_machines=_am->ramdom_select_K(2);
					for(auto om:other_machines){

						network_udp::generate_msg(msg_send_buffer,msg_t::QUERY,m.c_str());
						network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,om.c_str());

						_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source);
						msg_t indirectmsgtype=network_udp::get_response(msg_receive_buffer,additional_ip_received);

						if(string(source)==om && string(additional_ip_received)==m && indirectmsgtype==msg_t::QUERY_SUCCESS){
							failflag=false;
							break;
						}
					}
					if(failflag){
						_am->remove(m);
						_logger->add_write_log_task("Detector: remove "+m+" from membership list");
						_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
						for(auto om:alivemembers){
							if(om!=m){
								network_udp::generate_msg(msg_send_buffer,msg_t::FAIL,m.c_str());
								network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,om.c_str());
								// network_udp::send_msg(msg_t::FAIL,SERVERPORT,om.c_str());
								// _nw->recv_msg(source);
								// char tmp[INET6_ADDRSTRLEN];
								// strcpy(tmp,m.c_str());
								// network_udp::send_msg(m.c_str(),INET6_ADDRSTRLEN,SERVERPORT,om.c_str());
								// _nw->recv_msg(source);
							}
						}
					}


				}
			}
			sleep(DETECTOR_SLEEP_TIME_CONFIG);
		}
	}
	// EXIT
	// Send EXIT to all processes
	for(auto m:_am->get_alive_member()){
		source[0]='\0';
		network_udp::generate_msg(msg_send_buffer,msg_t::EXIT,m.c_str());
		network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,m.c_str());
	}
	return nullptr;

}