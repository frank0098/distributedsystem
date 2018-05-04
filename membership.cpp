#include "membership.h"

void Membership::run_membership_server(){
	char source[30];
	char source_port[30];
	char info[30];
	char info_port[30];
	while(_running){
		msg_t msgtype=_nw_server->recv_message(source,source_port,info,info_port);
		msg_t ret_msgtype=msg_t::UNKNOWN;
		switch(msgtype){
			case msg_t::JOIN:
			break;
			case msg_t::EXIT:
			break;
			case msg_t::PING:
			if(!Network_UDP::send_message(msg_t::ACK,_hostname.c_str(),_m_server_port.c_str(),info,info_port)){
				logger->write("FATAL:UNABLE TO SEND, PING");
			}
			break;
			case msg_t::INDIRECT_PING:
			if(!Network_UDP::send_message(msg_t::INDIRECT_ACK,_hostname.c_str(),_m_server_port.c_str(),info,info_port)){
				logger->write("FATAL:UNABLE TO SEND,INDIRECT_PING");
			}
			break;
			default:
			break;
		}
	}
}