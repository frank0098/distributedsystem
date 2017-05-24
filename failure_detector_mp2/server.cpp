#include "server.h"
server::server(loggerThread* lg):_lg(lg){
	_nw=new network_server();
	_nw->connect();
}
server::~server(){
	
}
void* server::run(){
	struct sockaddr_storage their_addr;
	int new_fd;
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	while(1){
		sin_size=sizeof(their_addr);
		new_fd=accept(_nw->get_fd(),(struct sockaddr*)&their_addr,&sin_size);
		if(new_fd==-1){
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		cout<<"server:got connection from"<<s<<endl;


		msg_t msg_type=_nw->recv_msg();
		msg_t response_type=msg_t::UNKNOWN;
		if(!fork()){
				close(_nw->get_fd());
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
				network_server::server_send(new_fd,response_type);
				_lg->add_write_log_task("SEND ACK TO "+string(s));
				close(new_fd);
				exit(0);
			}

		}
	close(new_fd);
}
