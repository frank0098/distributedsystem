
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


#define CMD_BUF_SIZE 200
using namespace std;

int main(int argc, char *argv[])
{


    if (argc != 3) {
        fprintf(stderr,"usage: client.out option params\n");
        exit(1);
    }

    try{
        vector<connection> vc;
        vector<string> ip_addrs=server_addr_read_config("server.cfg");
        for(auto ip:ip_addrs){
            vc.push_back(connection{ip});
        }


        if(ip_addrs.empty()){
            perror("empty config file\n");
            return -1;
        }
        char cmd_buf[CMD_BUF_SIZE];
        strcpy(cmd_buf,argv[1]);
        strcat(cmd_buf," ");
        strcat(cmd_buf,argv[2]);
        connect_all(vc);
        grep_all(vc,cmd_buf);
        disconnect_all(vc);
        print_all(vc);
    }
    catch(std::runtime_error err){
        cout<<"runtime_error"<<err.what()<<endl;
    }
    


    return 0;
}