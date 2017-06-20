#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <libgen.h>
#include <list>

#include "network.h"
#include "logger.h"

bool check_valid_ip(const char* ip){
	struct sockaddr_in sa;
	if(inet_pton(AF_INET,ip,&(sa.sin_addr))==0 && inet_pton(AF_INET6,ip,&(sa.sin_addr))==0) return false;
	return true;
}
void server_addr_read_config(std::string config_file_path,std::list<std::string>& all_member){
	std::ifstream in(config_file_path);
	std::string ip_addr;
	while(in>>ip_addr){
		if(!check_valid_ip(ip_addr.c_str())){
			throw std::runtime_error("invalid ip_addr: "+ip_addr);
		}
		else{
			all_member.push_back(ip_addr);
		}
	}
}


int main(int argc,char **argv){
	if (argc!=3){
		fprintf(stderr,"usage: ./remotecontroller.out restart|pause|kill IP_addr|all \n");
	    exit(1);
	}
	msg_t msgtype;
	if(strcmp(argv[1],"restart")==0){
		msgtype=msg_t::CONTROLLER_RESTART_REMOTELY;
	}
	else if(strcmp(argv[1],"pause")==0){
		msgtype=msg_t::CONTROLLER_PAUSE_REMOTELY;
	}
	else if(strcmp(argv[1],"kill")==0){
		msgtype=msg_t::CONTROLLER_END_REMOTELY;
	}
	else{
		fprintf(stderr,"usage: ./remotecontroller.out RESTART|PAUSE|KILL IP_addr|ALL \n");
		exit(1);
	}
	if(strcmp(argv[2],"all")==0){
		std::list<std::string> all_member;
		server_addr_read_config("server.cfg",all_member);
		for(auto am:all_member){
			network_udp::send_msg(msgtype,SERVICEPORT,am.c_str());
		}
	}
	else if(check_valid_ip(argv[2])){
		network_udp::send_msg(msgtype,SERVICEPORT,argv[2]);
	}
	else{
		fprintf(stderr,"usage: ./remotecontroller.out restart|pause|kill IP_addr|all \n");
    	exit(1);	
	}
	

	return 0;
}