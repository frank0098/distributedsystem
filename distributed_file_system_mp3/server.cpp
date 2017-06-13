#include "server.h"
server::server(loggerThread* lg,alive_member* am):_lg(lg),_am(am){
	_lg->add_write_log_task("server start");
	_nw=new network_udp(SERVERPORT,false);
	_nw->connect();
}
server::~server(){
	_nw->disconnect();
	delete _nw;
	_lg->add_write_log_task("server ends");
}

network_udp* server::get_nw(){
	return _nw;
}

void* server::run(){
	while(true){
		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		// _lg->add_write_log_task("SERVER: coordinator: "+coordinator);
		// pause_flag.lock();
		// while(pause_flag.is_true()){
		// 	pause_flag.cond_wait();
		// }
		// pause_flag.unlock();

		char source[INET6_ADDRSTRLEN];
		char msg_receive_buffer[BUFFER_SIZE];
		char additional_ip_received[INET6_ADDRSTRLEN];
		char msg_send_buffer[BUFFER_SIZE];
		msg_t msg_type;
		if(_nw->recv_msg(msg_receive_buffer,BUFFER_SIZE,source)){
			msg_type=network_udp::get_response(msg_receive_buffer,additional_ip_received);
		}
		else{
			msg_type=msg_t::TIMEOUT;
		}

		if(msg_type==msg_t::JOIN){
			if(_am->add(string(source))){
				_lg->add_write_log_task("Server: receive JOIN, Add "+string(source)+" to membership list");
				_lg->add_write_log_task("Server: current members: "+_am->get_alive_member_list());
			}
			else{
				_lg->add_write_log_task("Server: "+string(source)+" already in the membership list");
			}
			network_udp::generate_msg(msg_send_buffer,msg_t::JOIN_SUCCESS,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,source);
		}
		else if(msg_type==msg_t::EXIT){
			_am->remove(string(source));
			_lg->add_write_log_task("Server: Receive EXIT: Remove "+string(source)+" From membership list.");
			_lg->add_write_log_task("Server: current members: "+_am->get_alive_member_list());
			network_udp::generate_msg(msg_send_buffer,msg_t::ACK,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,source);

		}
		else if(msg_type==msg_t::PING){
			if(!_am->exists(string(source))){
				_am->add(string(source));
				_lg->add_write_log_task("Server: REJOIN Add "+string(source)+" to membership list");
			}
			// _lg->add_write_log_task("Server: current members: "+_am->get_alive_member_list());
			network_udp::generate_msg(msg_send_buffer,msg_t::ACK,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,source);
		}
		else if(msg_type==msg_t::QUERY){
			_lg->add_write_log_task("SERVER: receive QUERY from "+string(source)+" to ping "+additional_ip_received);
			network_udp::generate_msg(msg_send_buffer,msg_t::INDIRECT_PING,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,additional_ip_received);
		}
		else if(msg_type==msg_t::INDIRECT_PING){
			network_udp::generate_msg(msg_send_buffer,msg_t::INDIRECT_ACK,additional_ip_received);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,SERVERPORT,source);
		}
		else if(msg_type==msg_t::INDIRECT_ACK){

			network_udp::generate_msg(msg_send_buffer,msg_t::QUERY_SUCCESS,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,additional_ip_received);
		}
		else if(msg_type==msg_t::FAIL){
			char tmp_ip_addr[INET6_ADDRSTRLEN];
			_am->remove(string(additional_ip_received));
			_lg->add_write_log_task("SERVER: Receive FAIL from "+ string(source)+": Remove "+string(additional_ip_received)+" From membership list.");
			_lg->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
			network_udp::generate_msg(msg_send_buffer,msg_t::FAIL,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,DETECTORPORT,source);	
		}
		else if(msg_type==msg_t::ELECTION){
			network_udp::generate_msg(msg_send_buffer,msg_t::ELECTION_OK,source);
			network_udp::send_msg(msg_send_buffer,BUFFER_SIZE,ELECTIONPORT,source);

			if(string(additional_ip_received)==failure_process) continue;
			
			failure_process=string(additional_ip_received);
			election_stop_flag.lock();
			election_listener_stop_flag.lock();
			if(election_stop_flag.is_true() && election_listener_stop_flag.is_true()){
				election_stop_flag.set_false();
				election_stop_flag.cond_signal();
			}
			election_listener_stop_flag.unlock();
			election_stop_flag.unlock();
		}
		else if(msg_type==msg_t::COORDINATOR){
			election_stop_flag.lock();
			election_listener_stop_flag.lock();
			election_stop_flag.set_true();
			election_listener_stop_flag.set_true();
			election_listener_stop_flag.unlock();
			election_stop_flag.unlock();
			coordinator=string(source);

		}

		// cout<<"server send "<<response_type<<endl;
		// #if DEBUG
		// _lg->add_write_log_task("SERVER:  send ACK to "+string(source));
		// #endif
		// network_udp::send_msg(response_type,DETECTORPORT,source);
			
	}

}
