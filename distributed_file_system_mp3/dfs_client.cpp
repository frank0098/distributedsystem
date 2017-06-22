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




bool network_client::file_server_client(char* filename,char* request_type) {
    char query[BUFFER_SIZE];
    char buf[BUFFER_SIZE+1];

    char info[30];
    char file_size[30];
    file_size[0]='\0';
    if(strcmp(request_type,"GET")==0) {
        std::ofstream outfile(filename);
        outfile.close();
        outfile.open(filename, std::ios_base::app);
        sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0","");

        if((send(_sockfd,query,BUFFER_SIZE,0))<0) {
            perror("cannot send query");
            return false;
        }
        if(recv(_sockfd,buf,BUFFER_SIZE,0)==-1) {
            perror("recv");
            return false;
        }
        sscanf(buf, server_response_msg, info,filename,file_size);

    	if(strcmp(info,"200")!=0) return false;
        int data_recv=0;
        int file_size_left=atoi(file_size);
        while(file_size_left>0) {
            buf[0]='\0';
            int cur_recv=recv(_sockfd,buf,std::min(BUFFER_SIZE,file_size_left),0);
            cout<<"buf"<<cur_recv<<endl;
            if(cur_recv<=0) {
                perror("file_server_recv get");
                return false;
            }
            file_size_left-=cur_recv;
            buf[cur_recv]='\0';
            outfile<<buf;
        }
        outfile.close();

    }
    else if(strcmp(request_type,"POST")==0) {
        std::ifstream fin(filename,std::ios_base::in);
        if(fin.is_open()) {
        	struct stat st;
	        stat(filename,&st);
	        int file_size_number=st.st_size;
	        int file_size_left=file_size_number;
	        sprintf(file_size, "%d", file_size_number);
        	sprintf(query,client_msg,request_type,basename(filename),USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,file_size,"");

            if((send(_sockfd,query,BUFFER_SIZE,0))<0) {
                perror("cannot send query");
                return false;
            }
            if(recv(_sockfd,buf,BUFFER_SIZE,0)==-1) {
                perror("recv");
               	return false;
            }
            sscanf(buf, server_response_msg, info,filename,file_size);
            

    		if(strcmp(info,"200")!=0) return false;
            while(fin) {
                fin.read(buf,BUFFER_SIZE);
                int cur_send=send(_sockfd,buf,std::min(BUFFER_SIZE,file_size_left),0);
                if(cur_send<0) {
                    perror("cannot send");
                    return false;
                }
                file_size_left-=cur_send;
            }

        }
        else {
            perror("cannot open the file...\n");
            return false;
        }
    }
    else if(strcmp(request_type,"DELETE")==0){
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0","");
   		if((send(_sockfd,query,BUFFER_SIZE,0))<0) {
            perror("cannot send query");
            return false;
        }
        if(recv(_sockfd,buf,BUFFER_SIZE,0)==-1) {
            perror("recv");
            return false;
        }
        sscanf(buf, server_response_msg, info,filename,file_size);
        cout<<buf<<endl;
    	if(strcmp(info,"200")!=0) return false;
    }
    else if(strcmp(request_type,"LS")==0){
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0","");
    	if((send(_sockfd,query,BUFFER_SIZE,0))<0) {
            perror("cannot send query");
            return false;
        }
        if(recv(_sockfd,buf,BUFFER_SIZE,0)==-1) {
            perror("recv");
            return false;
        }
        sscanf(buf, server_response_msg, info,filename,file_size);
    	if(strcmp(info,"200")!=0) return false;
    	int file_size_number=atoi(file_size);
    	char res[file_size_number+1];
    	if(recv(_sockfd,res,file_size_number,0)==-1){
    		perror("recv");
    		return false;
    	}
    	res[file_size_number]='\0';
    	cout<<res<<endl;

    }

    return true;
}


int main(int argc,char ** argv){
	if (argc!=2){
		fprintf(stderr,"usage: ./client.out filename \n");
	    exit(1);
	}
	network_client* nw=new network_client("127.0.0.1",FILE_SERVER_PORT);
	nw->connect();
	if(nw->file_server_client(argv[1],"LS")==true){
		cout<<"file download complete: "<<argv[1]<<endl;
	}
	else{
		cout<<"error"<<argv[1]<<endl;
	}
	nw->disconnect();
	delete nw;
	return 0;
}