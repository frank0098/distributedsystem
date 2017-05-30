#include "network.h"

network::network(std::string hostname):_connected(false),_hostname(hostname),_sockfd(-1){

}
network_server::network_server():network(""){

}

network_client::network_client(std::string hostname):network(hostname){

}

network_udp::network_udp(const char* port,bool stm):network(""){
	_PORT=port;
	_settimeout=stm;
}

void network_udp::generate_msg(char* msg,msg_t msgtype,const char* ip_addr){
	// sscanf(msg,"Msg: /%s HTTP/1.1\r")
	msg[0]='\0';
	const char *format = "FailureDetector/1.0 \r\nMsg sent is %c \r\nAdditional info %s\r\n\r\n";
	sprintf(msg,format,(char)msgtype,ip_addr);
}
msg_t network_udp::get_response(char* msg,char* ip_addr){
	const char *format = "FailureDetector/1.0 \r\nMsg sent is %c \r\nAdditional info %s\r\n\r\n";
	msg_t msgtype;
	sscanf(msg,format,&msgtype,ip_addr);
	return msgtype;
}

bool network_udp::send_msg(msg_t msgtype,const char* port,const char* ip_addr){
		int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	// std::cout<<"send msg to "<<ip_addr<<" "<<port<<std::endl;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(ip_addr, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
		fprintf(stderr, "talker: failed to bind socket\n");
		return false;
	}

	if ((numbytes = sendto(sockfd, &msgtype, sizeof(msg_t), 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		return false;
	}

	freeaddrinfo(servinfo);
	close(sockfd);
	return true;
}

bool network_udp::send_msg(const char* msg,size_t msg_size,const char* port,const char* ip_addr){
		int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	// std::cout<<"send msg to "<<ip_addr<<" "<<port<<std::endl;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(ip_addr, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
		fprintf(stderr, "talker: failed to bind socket\n");
		return false;
	}

	if ((numbytes = sendto(sockfd, msg, msg_size, 0,
			 p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		return false;
	}

	freeaddrinfo(servinfo);
	close(sockfd);
	return true;
}

void network_udp::recv_msg(char* msg,size_t msg_size,char* ip_addr){
	msg[0]='\0';
	ip_addr[0]='\0';
	int numbytes;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	// msg_t msgtype=msg_t::UNKNOWN;
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(_sockfd, msg, msg_size , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		perror("recvfrom");
	}

	switch(their_addr.ss_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)((struct sockaddr *)&their_addr))->sin_addr),
                    ip_addr, INET6_ADDRSTRLEN);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)((struct sockaddr *)&their_addr))->sin6_addr),
                    ip_addr, INET6_ADDRSTRLEN);
            break;

        default:
            strncpy(ip_addr, "Unknown AF", INET6_ADDRSTRLEN);
    }
}

msg_t network_udp::recv_msg(char* ip_addr){
	ip_addr[0]='\0';
	int numbytes;
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	msg_t msgtype=msg_t::TIMEOUT;
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(_sockfd, &msgtype, sizeof(msg_t) , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		std::cout<<"err recv from"<<std::endl;
		perror("recvfrom");
		return msgtype;
	}

	switch(their_addr.ss_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)((struct sockaddr *)&their_addr))->sin_addr),
                    ip_addr, INET6_ADDRSTRLEN);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)((struct sockaddr *)&their_addr))->sin6_addr),
                    ip_addr, INET6_ADDRSTRLEN);
            break;

        default:
            strncpy(ip_addr, "Unknown AF", INET6_ADDRSTRLEN);
    }

	return msgtype;


}
void network_udp::connect(){

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 300000;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, _PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if(_settimeout && setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv))<0){
			perror("setsockopt");
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
		fprintf(stderr, "listener: failed to bind socket\n");
		return;
	}

	freeaddrinfo(servinfo);

	printf("listener: waiting to recvfrom...\n");
	_sockfd=sockfd;
	_connected=true;
}


bool network_server::server_send(int sockfd,msg_t msgtype){
	if(send(sockfd,&msgtype,sizeof(msgtype),0)==-1){
		perror("send error");
		return false;
	}
	return true;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s){
	int saved_errno=errno;
	while(waitpid(-1,NULL,WNOHANG)>0);
	errno=saved_errno;
}

void network_server::connect(){
	int sockfd,yes=1,rv;
	struct addrinfo hints,*servinfo,*p;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if((rv=getaddrinfo(NULL,PORT,&hints,&servinfo))!=0){
		fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));
		return;
	}
	for(p=servinfo;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("setsockopt");
			continue;
		}
		if(::bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
			close(sockfd);
			perror("server:bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if(!p){
		fprintf(stderr, "server:failed to bind\n" );
		exit(1);
	}
	struct sigaction sa;

	if(listen(sockfd,BACKLOG)==-1){
		perror("listen");
		exit(1);
	}
	sa.sa_handler=sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	if(sigaction(SIGCHLD,&sa,NULL)==-1){
		perror("sigaction");
		exit(1);
	}

	this->_sockfd=sockfd;
	this->_connected=true;

}

void network_client::connect(){
	int sockfd;  
	const char* ip_addr=_hostname.c_str();
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip_addr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure


    this->_sockfd=sockfd;
    this->_connected=true;

}

void network::disconnect(){
	int fd=_sockfd;
	this->_sockfd=-1;
	this->_connected=false;
	close(fd);
}

bool network::send_msg(msg_t type){
	if(send(_sockfd,&type,sizeof(msg_t),0)==-1){
		perror("send error");
		return false;
	}
	return true;
}

msg_t network_client::recv_msg(){
	msg_t msgtype;
	if(recv(_sockfd,&msgtype,sizeof (msg_t),0)<=0){
		perror("receive type fail");
		return msg_t::TIMEOUT;
	}
	return msgtype;
}

msg_t network_server::recv_msg(int nwesockfd){
	msg_t msgtype;
	if(recv(nwesockfd,&msgtype,sizeof (msg_t),0)<=0){
		perror("receive type fail");
		return msg_t::TIMEOUT;
	}
	return msgtype;
}

int network::get_fd(){
	return _sockfd;
}
bool network::is_connected(){
	return _connected;
}
std::string network::hostname(){
	return _hostname;
}