#include "server.h"
server::server(){

}
server::~server(){

}
void server::run(){
	struct sockaddr_storage their_addr;
	int new_fd;
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	while(1){
		sin_size=sizeof(their_addr);
		new_fd=accept(_nw->getfd(),(struct sockaddr*)&their_addr,&sin_size);
		if(new_fd==-1){
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		cout<<"server:got connection from"<<s<<endl;


		msg_t msg_type=get_msg_type(new_fd);

		if(!fork()){
				close(sockfd);
				if(msg_type==msg_t::JOIN){
					network::server_send(new_fd,msg_type::ACK);
				}
				else if(msg_type==msg_type::EXIT){
					network::server_send(new_fd,msg_type::ACK);
				}
				else if(msg_type==msg_type::PING){
					network::server_send(new_fd,msg_type::ACK);
				}
				else if(msg_type==msg_type::INDIRECT_PING){
					network::server_send(new_fd,msg_type::INDIRECT_ACK);
				}
				else if(msg_type==msg_type::INDIRECT_ACK){



				}
				else{
					network::server_send(new_fd,msg_type::UNKNOWN);
				}
				close(new_fd);
				exit(0);
			}

		}

		close(new_fd);
	}
}