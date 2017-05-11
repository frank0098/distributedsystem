
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
#include <iostream>
#include "utility.h"
using namespace std;

#define BACKLOG 10
#define PORT "3490"
#define MAXDATASIZE 200
void sigchld_handler(int s){
	int saved_errno=errno;
	while(waitpid(-1,NULL,WNOHANG)>0);
	errno=saved_errno;
}
void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family==AF_INET){
		return & (((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void){
	int sockfd,new_fd,rv,numbytes, yes=1;
	struct addrinfo hints,*servinfo,*p;
	struct sockaddr_storage their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	char buf[MAXDATASIZE];
	memset(&hints,0,sizeof(hints));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	if((rv=getaddrinfo(NULL,PORT,&hints,&servinfo))!=0){
		fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));
		return 1;
	}
	for(p=servinfo;p!=NULL;p=p->ai_next){
		if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("server socket");
			continue;
		}
		if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
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
	cout<<"server:waiting for connections"<<endl;
	while(1){
		sin_size=sizeof(their_addr);
		new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);
		if(new_fd==-1){
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		cout<<"server:got connection from"<<s<<endl;
		if((numbytes=recv(new_fd,buf,sizeof(buf)-1,0))==-1){
			perror("recv");
			exit(1);
		}
		buf[numbytes]='\0';
		cout<<"server received:"<<buf<<endl;

		const string grep_result=grep_server(buf);
		if(!fork()){
			close(sockfd);
			 char* thing_to_send=new char(grep_result.size()+2);
			 
			 strcpy(thing_to_send,grep_result.c_str());
			 thing_to_send[grep_result.size()]='\0';
			if(send(new_fd,thing_to_send,strlen(thing_to_send),0)==-1){
				perror("send");
			}
			delete[] thing_to_send;
			close(new_fd);
			exit(0);
		}
		close(new_fd);
	}
	return 0;

}