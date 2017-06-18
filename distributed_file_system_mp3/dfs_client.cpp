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
#include "network.h"

int main(int argc,char ** argv){
	if (argc!=2){
		fprintf(stderr,"usage: ./client.out filename \n");
	    exit(1);
	}
	network_client* nw=new network_client("127.0.0.1",FILE_SERVER_PORT);
	nw->connect();
	if(nw->file_server_client(argv[1])){
		cout<<"file download complete: "<<argv[1]<<endl;
	}
	else{
		cout<<"error"<<argv[1]<<endl;
	}
	nw->disconnect();
	delete nw;
	return 0;
}