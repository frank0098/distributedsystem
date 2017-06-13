#include "election.h"

election::election(loggerThread* lg):_lg(lg){
	_lg->add_write_log_task("election start");
	election_stop_flag.lock();
	election_stop_flag.set_true();
	election_stop_flag.unlock();
}
server::~server(){
	_lg->add_write_log_task("election ends");
}

void* server::run(){
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
		}
		else{
			for(auto p:machines_info){
				if(p.id>machine_id){
					network_udp::generate_msg(msg_send_buffer,msg_t::ELECTION,source);
					network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,source);
				}
			}
		}
		election_stop_flag.lock();
		election_stop_flag.set_true();
		election_stop_flag.unlock();
	}
}