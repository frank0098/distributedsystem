#include "server.h"
server::server(loggerThread* lg):_lg(lg){
	_lg->add_write_log_task("server start");
	_nw=new network_udp(SERVERPORT);
	_nw->connect();
}
server::~server(){
	_nw->disconnect();
	delete _nw;
}
void* server::run(){

	while(1){
		char source[INET6_ADDRSTRLEN];
		msg_t msg_type=_nw->recv_msg(source);
		_lg->add_write_log_task("SERVER:  recv msg from "+string(source));
		msg_t response_type=msg_t::UNKNOWN;
		// if(!fork())
		{
				// _nw->disconnect();
				if(msg_type==msg_t::JOIN){
					response_type=msg_t::ACK;
				}
				else if(msg_type==msg_t::EXIT){
					response_type=msg_t::ACK;
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
				cout<<"send ack!!aaa "<<source<<endl;
				network_udp::send_msg(response_type,DETECTORPORT,source);
				_lg->add_write_log_task("SERVER: SEND ACK TO "+string(source));
			}
	}

}
