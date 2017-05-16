
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include "utility.h"


#define MAXDATASIZE 2000000 // max number of bytes we can get at once 
#define CMD_BUF_SIZE 200
using namespace std;
// get sockaddr, IPv4 or IPv6:


int distrbuted_grep(const char* ip_addr,const char* cmd){


    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    sockfd=get_connection(ip_addr);
    if(sockfd==-1){
        perror("socket error\n");
        return -1;
    }
    if(send(sockfd,cmd,strlen(cmd),0)==-1){
        perror("send error\n");
       return -1;
    }
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv\n");
        return -1;
    }
    buf[numbytes] = '\0';
    printf("\n ==========client:received from %s =============\n",ip_addr);
    printf("%s\n\n",buf);
    close(sockfd);
    return 0;

}
int main(int argc, char *argv[])
{


    if (argc != 3) {
        fprintf(stderr,"usage: client.out option params\n");
        exit(1);
    }

    try{
        vector<string> ip_addrs=server_addr_read_config("server.cfg");

        if(ip_addrs.empty()){
            perror("empty config file\n");
            return -1;
        }
        char cmd_buf[CMD_BUF_SIZE];
        strcpy(cmd_buf,argv[1]);
        strcat(cmd_buf," ");
        strcat(cmd_buf,argv[2]);
        for(auto ip:ip_addrs){
            cout<<ip<<endl;
            distrbuted_grep(ip.c_str(),cmd_buf);
        }
    }
    catch(std::runtime_error err){
        cout<<"runtime_error"<<err.what()<<endl;
    }
    


    return 0;
}