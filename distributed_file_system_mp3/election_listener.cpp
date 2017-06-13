#include "election.h"

election_listener::election_listener(loggerThread* lg,alive_member *am):_lg(lg),_am(am){
	_lg->add_write_log_task("election start");
	_nw=new network_udp(ELECTIONPORT,true);
	_nw->connect();
	election_listener_stop_flag.lock();
	election_listener_stop_flag.set_true();
	election_listener_stop_flag.unlock();
}
election_listener::~election_listener(){
	_nw->disconnect();
	delete _nw;
	_lg->add_write_log_task("election ends");
}

void* election_listener::run(){
	char source[INET6_ADDRSTRLEN];
	char msg_receive_buffer[BUFFER_SIZE];
	char additional_ip_received[INET6_ADDRSTRLEN];
	char msg_send_buffer[BUFFER_SIZE];
	while(true){
		_lg->add_write_log_task("Election listner ongoing");
		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		election_listener_stop_flag.lock();
		while(election_listener_stop_flag.is_true()){
			election_listener_stop_flag.cond_wait();
		}
		election_listener_stop_flag.unlock();

		msg_t msg_type=msg_t::TIMEOUT;
		source[0]='\0';
		msg_receive_buffer[0]='\0';
		additional_ip_received[0]='\0';
		int higher_cnt=0;
		int higher_total=0;
		for(auto p:_am->get_alive_member_with_id()){
			if(p.id>machine_id){
				msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
				if(msg_type==msg_t::ELECTION_OK){
					higher_cnt++;
				}
				higher_total++;
			}
		}
		if(higher_total==0){
			_lg->add_write_log_task("BUG IN ELECTOR!!!!! machine id: "+std::to_string(machine_id));
		}
		// if receives some answers within timeout

		election_listener_stop_flag.lock();
		election_listener_stop_flag.set_true();
		election_listener_stop_flag.unlock();
		if(higher_cnt==0){
			_lg->add_write_log_task("ELECTOR LISTNER: coordinator: "+coordinator);
			coordinator=machine_ip;
			highest_id=machine_id;
		}
		else{
			election_stop_flag.lock();
			election_stop_flag.set_false();
			election_stop_flag.cond_signal();
			election_stop_flag.unlock();
		}

	}
}