#include "server.h"
server::server(loggerThread* lg,alive_member* am):_lg(lg),_am(am){
	_lg->add_write_log_task("server start");
	_nw=new network_udp(SERVERPORT,false);
	_nw->connect();
}
server::~server(){
	_nw->disconnect();
	delete _nw;
	cout<<"server ends"<<endl;
	// _lg->add_write_log_task("server ends");
}
void* server::run(){

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

		char source[INET6_ADDRSTRLEN];
		msg_t msg_type=_nw->recv_msg(source);
		// _lg->add_write_log_task("SERVER:  recv msg from "+string(source));
		// cout<<"server recv msg_type "<<msg_type<<endl;
		msg_t response_type=msg_t::UNKNOWN;
		// if(!fork())
				// _nw->disconnect();
		if(msg_type==msg_t::JOIN){
			response_type=msg_t::ACK;
			_lg->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
		}
		else if(msg_type==msg_t::EXIT){
			response_type=msg_t::ACK;
			_lg->add_write_log_task("SERVER: Remove "+string(source)+" From membership list.");
			_lg->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
			_am->remove(string(source));
		}
		else if(msg_type==msg_t::PING){
			response_type=msg_t::ACK;
		}
		else if(msg_type==msg_t::INDIRECT_PING){
			response_type=msg_t::INDIRECT_ACK;
		}
		else if(msg_type==msg_t::INDIRECT_ACK){


			response_type=msg_t::ACK;

		}
		else if(msg_type==msg_t::FAIL){
			response_type=msg_t::ACK;
			network_udp::send_msg(response_type,DETECTORPORT,source);
			char tmp_ip_addr[INET6_ADDRSTRLEN];
			_nw->recv_msg(tmp_ip_addr,INET6_ADDRSTRLEN,source);

			_lg->add_write_log_task("SERVER: Remove "+string(tmp_ip_addr)+" From membership list.");
			_lg->add_write_log_task("Detector: current members: "+_am->get_alive_member_list());
			_am->remove(string(tmp_ip_addr));

		}
		// cout<<"server send "<<response_type<<endl;
		#if DEBUG
		_lg->add_write_log_task("SERVER:  send ACK to "+string(source));
		#endif
		network_udp::send_msg(response_type,DETECTORPORT,source);
			
	}

}
