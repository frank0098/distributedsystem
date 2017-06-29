#include "election.h"

election::election(loggerThread* lg,alive_member *am):_lg(lg),_am(am){
	_lg->add_write_log_task("election start");
	election_stop_flag.lock();
	election_stop_flag.set_true();
	election_stop_flag.unlock();
}
election::~election(){
	_lg->add_write_log_task("election ends");
}

void* election::run(){

	char source[INET6_ADDRSTRLEN];
	char msg_receive_buffer[BUFFER_SIZE];
	char msg_send_buffer[BUFFER_SIZE];
	while(true){
		cout<<"election"<<endl;
		_lg->add_write_log_task("Election ongoing");
		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		election_stop_flag.lock();
		while(election_stop_flag.is_true()){
			election_stop_flag.cond_wait();
		}
		election_stop_flag.unlock();
		_lg->add_write_log_task("Election bypass");
		
		_lg->add_write_log_task("DEBUG machine id : "+std::to_string(machine_id) + " highest_id: "+std::to_string(highest_id));
		if(machine_id==highest_id){
			coordinator=machine_ip;
			_lg->add_write_log_task("ELECTOR coordinator: "+coordinator);
			for(auto p:_am->get_alive_member_with_id()){
				if(p.id<machine_id){
					network_udp::generate_msg(msg_send_buffer,msg_t::COORDINATOR,source);
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,(p.ip).c_str());
				}
			}
		}
		else{
			election_listener_stop_flag.lock();
			election_listener_stop_flag.set_false();
			election_listener_stop_flag.cond_signal();
			election_listener_stop_flag.unlock();
			for(auto p:_am->get_alive_member_with_id()){
				if(p.id>machine_id){
					network_udp::generate_msg(msg_send_buffer,msg_t::ELECTION,coordinator.c_str());
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,(p.ip).c_str());
				}
			}
		}
		election_stop_flag.lock();
		election_stop_flag.set_true();
		election_stop_flag.unlock();
	}
}