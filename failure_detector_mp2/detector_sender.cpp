#include "detector.h"

detector_sender::detector_sender(std::list<string> *mem, alive_member *am,loggerThread *lg):_members(mem),_am(am),_logger(lg){
	_logger->add_write_log_task("detector sender start");
}
detector_sender::~detector_sender(){
	_logger->add_write_log_task("detector sender ends");
	// cout<<"END DETECTOR"<<endl;
}	

void* detector_sender::run(){
	char source[INET6_ADDRSTRLEN];
	char msg_receive_buffer[BUFFER_SIZE];
	char additional_ip_received[INET6_ADDRSTRLEN];
	char msg_send_buffer[BUFFER_SIZE];
	detector_sender_stop_flag.lock();
	detector_stop_flag.lock();
	detector_sender_stop_flag.set_false();
	detector_stop_flag.set_true();
	detector_sender_stop_flag.unlock();
	detector_stop_flag.unlock();
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

		while(detector_sender_stop_flag.is_true()){
			detector_sender_stop_flag.cond_wait();
		}

		sleep(DETECTOR_SLEEP_TIME_CONFIG);
		
		detector_stop_flag.lock();
		detector_stop_flag.set_false();
		detector_stop_flag.cond_signal();
		detector_stop_flag.unlock();
		// cout<<"running : current state: "<<ds<<endl;
		_logger->add_write_log_task("running : current state: "+to_string((int)ds));
		if(ds==detector_state::START_PHASE){
			for(auto m:*_members){
			network_udp::generate_msg(msg_send_buffer,msg_t::JOIN,m.c_str());
			if(!network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,m.c_str())){
				_logger->add_write_log_task("Detector: FATAL ERROR TO CONNECT "+m);
				continue;
				}
			}

		}
		else if(ds==detector_state::PING_ACK_PHASE){
			for(auto m:_am->get_alive_member()){
			network_udp::generate_msg(msg_send_buffer,msg_t::PING,m.c_str());
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,m.c_str());
			}

		}
		else if(ds==detector_state::SUSPICIOUS){
			for(auto m:suspicious_dead_members){
				std::vector<std::string> other_machines=alive_member::random_select_K(2,m,_am->get_alive_member());
				if(other_machines.size()==0) continue;
				for(auto om:other_machines){
					network_udp::generate_msg(msg_send_buffer,msg_t::QUERY,m.c_str());
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,om.c_str());
				}
			}
		}

		detector_sender_stop_flag.lock();
		detector_stop_flag.set_true();
		detector_sender_stop_flag.unlock();	

		
	}

	for(auto m:_am->get_alive_member()){
		source[0]='\0';
		network_udp::generate_msg(msg_send_buffer,msg_t::EXIT,m.c_str());
		network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,m.c_str());
	}
	return nullptr;
} 