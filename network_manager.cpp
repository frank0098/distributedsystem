#include "network_manager.h"

#define BUFFER_SIZE 256

static const char *format = "FailureDetector/1.0 \r\nMsg sent is %c\r\nSource %s\r\nSourceport %s\r\nExtra1%s\r\nExtra2%s\r\n\r\n";

static void generate_msg(char* msg,msg_t msgtype,const char* source,const char* source_port,const char* info,const char* info_port){
	msg[0]='\0';
	sprintf(msg,format,(char)msgtype,source,source_port,info,info_port);
}
static msg_t get_response(char* msg,char* source,char* source_port,char* info,char* info_port){
	msg_t msgtype=msg_t::UNKNOWN;
	sscanf(msg,format,&msgtype,source,source_port,info,info_port);
	return msgtype;
}
Network::Network(){
	logger()->write("network module starts");
}

Network_UDP::Network_UDP(const char* hostname,const char* port):Network()
{
	strcpy(_hostname,hostname);
	strcpy(_port,port);
}
void Network_UDP::connect(){
	if(_sockfd>0) return;
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	// char portstr[10];
 //    sprintf(portstr, "%d", _port);
	if ((rv = getaddrinfo(NULL,_port, &hints, &servinfo))!= 0) {
		char err[BUFFER_SIZE];
		sprintf(err, "getaddrinfo: %s\n", gai_strerror(rv));
		logger()->write(err);
		return;
	}
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}
	if (p == NULL) {
		logger()->write("listener: failed to bind socket\n");
		return;
	}

	freeaddrinfo(servinfo);
	_sockfd=sockfd;

}
void Network_UDP::disconnect(){
	if(_sockfd<0) return;
	logger()->write("Network Module Disconnect\n");
	close(_sockfd);
	_sockfd=-1;
}

bool Network_UDP::send_message(msg_t type,const char* dest,const char* dest_port,const char* source,const char* source_port,const char* info,const char* info_port){

	char buffer[BUFFER_SIZE];
	generate_msg(buffer,type,source,source_port,info,info_port);

	int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(dest, dest_port, &hints, &servinfo)) != 0) {
        char err[BUFFER_SIZE];
		sprintf(err, "getaddrinfo: %s\n", gai_strerror(rv));
		logger()->write(err);
        return false;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
		logger()->write("talker: failed to bind socket\n");
        return false;
    }

    if ((numbytes = sendto(sockfd, buffer, BUFFER_SIZE, 0,
                           p->ai_addr, p->ai_addrlen)) == -1) {
    	logger()->write("talker: sendto\n");
        return false;
    }

    freeaddrinfo(servinfo);
    close(sockfd);
    return true;
}

msg_t Network_UDP::recv_message(char* source,char* source_port,char* info,char* info_port){
	if(_sockfd<0) return msg_t::UNKNOWN;
	char buffer[BUFFER_SIZE];
	int numbytes;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	msg_t msgtype=msg_t::UNKNOWN;
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(_sockfd, buffer, BUFFER_SIZE , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		logger()->write("recvfrom\n");
		return msgtype;
	}
	// cout<<"buff"<<buffer<<endl;
	msgtype=get_response(buffer,source,source_port,info,info_port);
	
	return msgtype;
}

void Network_UDP::wait_message_from_peers(vector<Peer_struct>& input){
	if(_sockfd<0) return;
	char buffer[BUFFER_SIZE];
	char source[30];
	char info[30];
	char source_port[30];
	char info_port[30];
	source[0]='\0';
	info[0]='\0';
	struct sockaddr_storage their_addr;
	socklen_t addr_len = sizeof their_addr;

	fd_set active_fd_set, read_fd_set;

	FD_ZERO (&active_fd_set);
  	FD_SET (_sockfd, &active_fd_set);
  	struct timeval tv;
  	tv.tv_sec=3;
	tv.tv_usec=0;
	int cnt=0;
  	while(true)
  	{
  		read_fd_set = active_fd_set;
  		int selectStatus=select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv);
  		//printf("select status %d\n",selectStatus);
  		if (selectStatus < 0)
        {
          logger()->write("FATAL ERROR:SELECT\n");
          break;
          // exit (EXIT_FAILURE);
        }
        if(selectStatus==0){
        	// printf("select timeout \n");
        	break;
        }
    	if(FD_ISSET(_sockfd,&read_fd_set)){
			if (int numbytes = recvfrom(_sockfd, buffer, BUFFER_SIZE , 0,
				(struct sockaddr *)&their_addr, &addr_len) == -1) {
				// perror("recvfrom");
				logger()->write("recvfromerr");
				break;
				// exit(EXIT_FAILURE);
			}
			msg_t msgtype=get_response(buffer,source,source_port,info,info_port);

			// cout<<"RECVMESSAGE: "<<buffer<<endl;
			if(msgtype==msg_t::ACK){
				for(auto &p:input){
					if(strcmp(source,p.peerip)==0 && strcmp(source_port,p.peerport)==0){
						p.suspicious=false;
						break;
					}
				}
			}
			else{
				logger()->write("BUG:Unexpected msg: "+std::to_string(msgtype-'a'));
			}
    		
    		FD_CLR(_sockfd, &read_fd_set);
    	}
        
  	}
}




