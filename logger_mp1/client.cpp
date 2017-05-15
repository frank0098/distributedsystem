
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
#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 2000000 // max number of bytes we can get at once 
#define CMD_BUF_SIZE 200
using namespace std;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int distrbuted_grep(const char* ip_addr,const char* cmd){


    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip_addr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    if(send(sockfd,cmd,strlen(cmd),0)==-1){
        perror("send error\n");
        exit(1);
    }
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv\n");
        exit(1);
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