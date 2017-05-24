#include "network.h"

network::network(std::string hostname):_connected(false),_hostname(hostname),_sockfd(-1){

}
network_server::network_server():network(""){

}

network_client::network_client(std::string hostname):network(hostname){

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

bool network::send_msg(msg_t type){
	if(send(_sockfd,&type,sizeof(msg_t),0)==-1){
		perror("send error");
		return false;
	}
	return true;
}

msg_t network::recv_msg(){
	msg_t msgtype;
	if(recv(_sockfd,&msgtype,sizeof (msg_t),0)<=0){
		perror("receive type fail");
		return msg_t::UNKNOWN;
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