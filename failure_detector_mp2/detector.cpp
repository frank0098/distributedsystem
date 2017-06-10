#include "detector.h"


flag_t detector_sender_stop_flag;
flag_t detector_stop_flag;
detector_state ds=detector_state::START_PHASE;
std::vector<std::string> suspicious_dead_members;


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
			ds=detector_state::START_PHASE;
		}
		pause_flag.unlock();

		detector_stop_flag.lock();
		while(detector_stop_flag.is_true()){
			detector_stop_flag.cond_wait();
		}
		detector_stop_flag.unlock();

		msg_t msg_type=msg_t::TIMEOUT;
		source[0]='\0';
		msg_receive_buffer[0]='\0';
		additional_ip_received[0]='\0';

		if(ds==detector_state::START_PHASE){

			for(auto m:*_members){

				if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
					msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				}
				if(msg_type==msg_t::JOIN_SUCCESS){
					if(_am->add(string(source))){
							_logger->add_write_log_task("Detector: Add "+string(source)+" to membership list");
							_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
					}
					else{
						_logger->add_write_log_task("Detector: "+string(source)+" already in the membership list");
					}
					cout<<source<<endl;
					ds=detector_state::PING_ACK_PHASE;
				}   

					
			}

			
		}
		else if(ds==detector_state::PING_ACK_PHASE){
			suspicious_dead_members=_am->get_alive_member();
			for(auto m:_am->get_alive_member()){
				if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
					msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				}
				if(msg_type==msg_t::ACK){
					auto it = std::find(suspicious_dead_members.begin(), suspicious_dead_members.end(), string(source));
					if(it != suspicious_dead_members.end())
					    suspicious_dead_members.erase(it);
				}
			}
			if(suspicious_dead_members.size()!=0){
				ds=detector_state::SUSPICIOUS;
			}

		}
		else if(ds==detector_state::SUSPICIOUS){
			for(auto m:suspicious_dead_members){
				if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
					msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				}
				if(msg_type==msg_t::QUERY_SUCCESS){
					auto it = std::find(suspicious_dead_members.begin(), suspicious_dead_members.end(), string(source));
					if(it != suspicious_dead_members.end())
					    suspicious_dead_members.erase(it);
				}
			}
			for(auto m:suspicious_dead_members){
				_am->remove(m);
				_logger->add_write_log_task("Detector: remove "+m+" from membership list");
				_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
			}
			ds=detector_state::PING_ACK_PHASE;
		}
		detector_stop_flag.lock();
		detector_stop_flag.set_true();
		detector_stop_flag.unlock();
	}
	return nullptr;
}
		
		// if(msg_type==msg_t::JOIN_SUCCESS){

		// 	if(_am->add(m)){
		// 			_logger->add_write_log_task("Detector: Add "+string(source)+" to membership list");
		// 			_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
		// 	}
		// 	else{
		// 			_logger->add_write_log_task("Detector: "+string(source)+" already in the membership list");
		// 	}
		// }   
		// else if(msg_type==msg_t::ACK) continue;


		// else if(msg_type==msg_t::TIMEOUT){
		// 	_logger->add_write_log_task("Detector: receive " + to_string((char)(msg_type))+" :"+m+" might have FAILED.SEND QUERY");
		// 	bool failflag=true;
		// 	std::vector<std::string> other_machines=_am->ramdom_select_K(2);
		// 	if(other_machines.size()==0) continue;
		// 	for(auto om:other_machines){

		// 		network_udp::generate_msg(msg_send_buffer,msg_t::QUERY,m.c_str());
		// 		network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,om.c_str());
		// 		msg_t indirectmsgtype=msg_t::TIMEOUT;
		// 		if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
		// 			indirectmsgtype=network_udp::get_response(msg_receive_buffer,additional_ip_received);
		// 		}

		// 		if(string(source)==om && string(additional_ip_received)==m && indirectmsgtype==msg_t::QUERY_SUCCESS){
		// 			failflag=false;
		// 			break;
		// 		}
		// 	}
		// 	if(failflag){
		// 		_am->remove(m);
		// 		_logger->add_write_log_task("Detector: remove "+m+" from membership list");
		// 		_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
		// 		for(auto om:alivemembers){
		// 			if(om!=m){
		// 				network_udp::generate_msg(msg_send_buffer,msg_t::FAIL,m.c_str());
		// 				network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,om.c_str());
		// 			}
		// 		}
		// 	}
		// }
		// else if(msg_type==msg_t::QUERY_SUCCESS){
		// 	continue;
		// }
		// else{
		// 	_logger->add_write_log_task("Detector: ===  ERROR ===  recv msg from "+m);
		// }

		


