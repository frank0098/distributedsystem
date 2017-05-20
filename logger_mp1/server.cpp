
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
#include <string.h>
#include "utility.h"

using namespace std;

#define BACKLOG 10
#define PORT "3490"

void sigchld_handler(int s){
	int saved_errno=errno;
	while(waitpid(-1,NULL,WNOHANG)>0);
	errno=saved_errno;
}
int main(void){
	int sockfd;
	sockfd=server_setup_connection();

	struct sigaction sa;

	// char buf[MAXDATASIZE];

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
	try{
		serve_forever(sockfd);
	}
	catch(std::runtime_error err){
        cout<<"runtime_error"<<err.what()<<endl;
    }
	close(sockfd);

	return 0;

}