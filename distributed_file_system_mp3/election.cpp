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
	char additional_ip_received[INET6_ADDRSTRLEN];
	char msg_send_buffer[BUFFER_SIZE];
	while(true){
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

		if(machine_id==highest_id){
			coordinator=machine_ip;
			_lg->add_write_log_task("coordinator: "+coordinator);
			for(auto p:_am->get_alive_member_with_id()){
				if(p.id<machine_id){
					network_udp::generate_msg(msg_send_buffer,msg_t::COORDINATOR,source);
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,source);
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
					network_udp::generate_msg(msg_send_buffer,msg_t::ELECTION,source);
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,source);
				}
			}
		}
		election_stop_flag.lock();
		election_stop_flag.set_true();
		election_stop_flag.unlock();
	}
}