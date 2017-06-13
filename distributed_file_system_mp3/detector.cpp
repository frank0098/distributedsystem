#include "detector.h"



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

		_logger->add_write_log_task("DETECTOR DEBUG machine id : "+std::to_string(machine_id) + " highest_id: "+std::to_string(highest_id));
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

		// _logger->add_write_log_task("running : detector current state: "+to_string((int)ds));

		if(ds==detector_state::START_PHASE){

			for(auto m:*_members){

				if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
					msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				}

				_logger->add_write_log_task("Detector: Recv msgtype "+to_string(msg_type)+" from "+string(source));
				if(msg_type==msg_t::JOIN_SUCCESS){
					if(_am->add(string(source))){
							_logger->add_write_log_task("Detector: Add "+string(source)+" to membership list");
							_logger->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
					}
					else{
						_logger->add_write_log_task("Detector: "+string(source)+" already in the membership list");
					}
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
				else{
					_logger->add_write_log_task("Detector: recv msg: "+to_string(msg_type));
				}
			}
			if(suspicious_dead_members.size()!=0){
				ds=detector_state::SUSPICIOUS;
				string sus_mem;
				for(auto m:suspicious_dead_members){
					sus_mem+=m;
					sus_mem+=" ";
				}
				_logger->add_write_log_task("Detector: suspect dead members: "+sus_mem);

			}

		}
		else if(ds==detector_state::SUSPICIOUS){
			_logger->add_write_log_task("DETECTOR Suspicious state");
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
				if(m==coordinator){
					election_stop_flag.lock();
					election_listener_stop_flag.lock();
					if(election_stop_flag.is_true() && election_listener_stop_flag.is_true()){
						failure_process=m;
						election_stop_flag.set_false();
						election_stop_flag.cond_signal();
					}
					election_listener_stop_flag.unlock();
					election_stop_flag.unlock();
				}
			}
			ds=detector_state::PING_ACK_PHASE;
		}
		detector_sender_stop_flag.lock();
		detector_sender_stop_flag.set_false();
		detector_sender_stop_flag.cond_signal();
		detector_sender_stop_flag.unlock();
		detector_stop_flag.lock();
		detector_stop_flag.set_true();
		detector_stop_flag.unlock();
	}
	return nullptr;
}



