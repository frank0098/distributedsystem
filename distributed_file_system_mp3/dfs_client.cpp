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
#include <cstdlib> 
#include <ctime> 
#include "network.h"


void server_addr_read_config(string config_file_path,std::vector<string>& all_member){
	std::ifstream in(config_file_path);
	string ip_addr;
	struct sockaddr_in sa;
	int i=0;
	while(in>>ip_addr){
		const char* tmp_addr=ip_addr.c_str(); 
		if(inet_pton(AF_INET,tmp_addr,&(sa.sin_addr))==0 &&
		 inet_pton(AF_INET6,tmp_addr,&(sa.sin_addr))==0){
			throw std::runtime_error("invalid ip_addr: "+ip_addr);
		}
		else{
			all_member.push_back(ip_addr);
		}
	}
  	
}

bool network_client::file_server_client(char* filename,const char* request_type,char* msg) {
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
    else if(strcmp(request_type,"COORDINATOR")==0 || strcmp(request_type,"GET_FILE_ADDR_ONE")==0 
    	|| strcmp(request_type,"GET_FILE_ADDR_ALL")==0 || strcmp(request_type,"REQUEST_POST_FILE")==0
    	|| strcmp(request_type,"LIST_ALL_FILES")==0){
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0","");
    	if((send(_sockfd,query,BUFFER_SIZE,0))<0) {
            perror("cannot send query");
            return false;
        }
        if(recv(_sockfd,msg,BUFFER_SIZE,0)==-1){
        	perror("recv");
        	return false;
        }

    }
    else if(strcmp(request_type,"GET_FILE_ADDR_ONE")==0){
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0","");
    	

    }

    return true;
}

void file_op(char* filename,char* request_type){
	std::vector<string> members;
	server_addr_read_config("server.cfg",members);
	srand((unsigned)time(0)); 
	int sze=members.size();
	int rnd=rand()%sze;
	char coordinator[INET6_ADDRSTRLEN];
	network_client* nw=nullptr;
	for(int i=0;i<members.size();i++){
		nw=new network_client(members[(i+rnd)%sze].c_str(),FILE_SERVER_PORT);
		nw->connect();
		if(nw->file_server_client("","COORDINATOR",coordinator)==false){
			nw->disconnect();
			delete nw;
			continue;
		}
		else break;

	}
	if(nw!=nullptr){
		nw->disconnect();
		delete nw;
	}
	if(strcmp(request_type,"GET")==0){
		nw=new network_client(coordinator,FILE_SERVER_PORT);
		nw->connect();
		char file_ip[INET6_ADDRSTRLEN];
		if(nw->file_server_client("","GET_FILE_ADDR_ONE",file_ip)==false){
			cout<<"GET_FILE_ADDR_ONE from coordinator "<<coordinator<<" for file"<<filename<<" FAIL!"<<endl;
		}
		nw->disconnect();
		delete nw;
		nw=new network_client(file_ip,FILE_SERVER_PORT);
		nw->connect();
		if(nw->file_server_client(filename,"GET","")){

			cout<<"GET file "<<filename<<" from "<<file_ip<<" Successfully!"<<endl;
		}
		else{
			cout<<"GET file "<<filename<<" from "<<file_ip<<" FAIL!"<<endl;
		}
		nw->disconnect();
		delete nw;
	}
	else if(strcmp(request_type,"POST")==0) {
		nw=new network_client(coordinator,FILE_SERVER_PORT);
		nw->connect();
		char file_ip_addr[BUFFER_SIZE];
		if(nw->file_server_client("","GET_FILE_ADDR_ALL",file_ip_addr)==false){
			cout<<"GET_FILE_ADDR_ALL from coordinator "<<coordinator<<" for file"<<filename<<" FAIL!"<<endl;
		}
		nw->disconnect();
		delete nw;
		if(strcmp(file_ip_addr,"404")==0){
			if(nw->file_server_client("","REQUEST_POST_FILE",file_ip_addr)==false){
			cout<<"REQUEST_POST_FILE FAIL from coordinator "<<coordinator<<" from file"<<filename<<" FAIL!"<<endl;
			}
		string ip_addrs=string(file_ip_addr);
		std::vector<string> v_ip;
		int loc=0;
		for(int i=0;i<ip_addrs.size();i++){
			if(ip_addrs[i]=='\t'){
				v_ip.push_back(ip_addrs.substr(loc,i-loc));
				loc=i+1;
			}
		}
		if(!fork()){
			for(auto ip:v_ip){
				network_client* nnw=new network_client(ip,FILE_SERVER_PORT);
				nnw->connect();
				if(nw->file_server_client(filename,"POST",file_ip_addr)==false){
					cout<<"POST FAIL to fileserver "<<ip<<" for file"<<filename<<" FAIL!"<<endl;
				}
				else{
					cout<<"POST SUCCESS to fileserver "<<ip<<" for file"<<filename<<" SUCCESS!"<<endl;
				}
				nnw->disconnect();
				delete nnw;
			}
			exit(0);
		}

		for(auto ip:members){
			nw=new network_client(ip,FILE_SERVER_PORT);
			nw->connect();
			if(nw->file_server_client(filename,"INSERT_FILE_ENTRY",file_ip_addr)){
				cout<<"INSERT_FILE_ENTRY"<<filename<<"to"<<ip<< "entry"<<file_ip_addr<<" Successfully!"<<endl;
			}
			else{
				cout<<"INSERT_FILE_ENTRY"<<filename<<"to"<<ip<< "entry"<<file_ip_addr<<" FAIL!"<<endl;
			}
			nw->disconnect();
			delete nw;

		}

		}

	}
	else if(strcmp(request_type,"DELETE")==0) {
		
	}
	else if(strcmp(request_type,"LS")==0) {
		nw=new network_client(coordinator,FILE_SERVER_PORT);
		nw->connect();
		char all_files[BUFFER_SIZE];
		if(nw->file_server_client("","LIST_ALL_FILES",all_files)==false){
			cout<<"LIST_ALL_FILES from coordinator "<<coordinator<<" FAIL!"<<endl;
		}
		else{
			cout<<"LIST_ALL_FILES from coordinator "<<coordinator<<" SUCCESS!"<<endl;
			cout<<all_files<<endl;
		}

		nw->disconnect();
		delete nw;


		
	}
	else if(strcmp(request_type,"STORE")==0) {
		for(auto m:members){
			nw=new network_client(m,FILE_SERVER_PORT);
			nw->connect();
			char all_files[BUFFER_SIZE];
			if(nw->file_server_client("","LS",all_files)){
				cout<<"Files in "<<m<<" :\n"<<all_files;
			}
			else{
				cout<<"Server "<<m<<" is unreachable"<<endl;
			}
			nw->disconnect();
			delete nw;

		}
	}
	


}


int main(int argc,char ** argv){
	if (argc!=3){
		fprintf(stderr,"usage: ./client.out ops [filename] \n");
	    exit(1);
	}
	network_client* nw=new network_client("127.0.0.1",FILE_SERVER_PORT);
	nw->connect();
	// if(nw->file_server_client(argv[1],"LS")==true){
	// 	cout<<"file download complete: "<<argv[1]<<endl;
	// }
	// else{
	// 	cout<<"error"<<argv[1]<<endl;
	// }
	nw->disconnect();
	delete nw;
	return 0;
}