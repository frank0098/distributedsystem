#include "network.h"

network::network(std::string hostname):_connected(false),_hostname(hostname),_sockfd(-1) {
    _lg=nullptr;
}

network::network(std::string hostname,loggerThread* lg):_connected(false),_hostname(hostname),_sockfd(-1),_lg(lg) {

}
network_server::network_server(const char* port,loggerThread* lg):network("",lg) {
    _PORT=port;
}

network_client::network_client(std::string hostname,const char* port):network(hostname) {
    _PORT=port;
}

network_udp::network_udp(const char* port,bool stm):network("") {
    _PORT=port;
    _settimeout=stm;
}

void network_udp::generate_msg(char* msg,msg_t msgtype,const char* ip_addr) {
    // sscanf(msg,"Msg: /%s HTTP/1.1\r")
    msg[0]='\0';
    const char *format = "FailureDetector/1.0 \r\nMsg sent is %c \r\nAdditional info %s\r\n\r\n";
    sprintf(msg,format,(char)msgtype,ip_addr);
}
msg_t network_udp::get_response(char* msg,char* ip_addr) {
    const char *format = "FailureDetector/1.0 \r\nMsg sent is %c \r\nAdditional info %s\r\n\r\n";
    msg_t msgtype=msg_t::TIMEOUT;
    sscanf(msg,format,&msgtype,ip_addr);
    return msgtype;
}

bool network_udp::send_msg(msg_t msgtype,const char* port,const char* ip_addr) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;

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

bool network_udp::send_msg(const char* msg,size_t msg_size,const char* port,const char* ip_addr) {
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

bool network_udp::recv_msg(char* msg,size_t msg_size,char* ip_addr) {
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
        return false;
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
    return true;
}

msg_t network_udp::recv_msg(char* ip_addr) {
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
void network_udp::connect() {

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct timeval tv;
    tv.tv_sec = 4;
    tv.tv_usec = 000000;
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

        if(_settimeout && setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv))<0) {
            perror("setsockopt");
            continue;
        }

        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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


// bool network_server::server_send(int sockfd,msg_t msgtype){
// 	if(send(sockfd,&msgtype,sizeof(msgtype),0)==-1){
// 		perror("send error");
// 		return false;
// 	}
// 	return true;
// }

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s) {
    int saved_errno=errno;
    while(waitpid(-1,NULL,WNOHANG)>0);
    errno=saved_errno;
}


void network_server::connect() {
    int sockfd,yes=1,rv;
    struct addrinfo hints,*servinfo,*p;

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;
    if((rv=getaddrinfo(NULL,PORT,&hints,&servinfo))!=0) {
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));
        return;
    }
    for(p=servinfo; p!=NULL; p=p->ai_next) {
        if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1) {
            perror("server socket");
            continue;
        }
        if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1) {
            perror("setsockopt");
            continue;
        }
        if(::bind(sockfd,p->ai_addr,p->ai_addrlen)==-1) {
            close(sockfd);
            perror("server:bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if(!p) {
        fprintf(stderr, "server:failed to bind\n" );
        exit(1);
    }
    struct sigaction sa;

    if(listen(sockfd,BACKLOG)==-1) {
        perror("listen");
        exit(1);
    }
    sa.sa_handler=sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    if(sigaction(SIGCHLD,&sa,NULL)==-1) {
        perror("sigaction");
        exit(1);
    }

    this->_sockfd=sockfd;
    this->_connected=true;

}

void network_client::connect() {
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

const char *server_response_msg = "HTTP/1.1 %s\r\nThe file requested is %s with SIZE %s\r\n\r\n";
const char *client_msg="%s %s HTTP/1.1\r\nUser-Agent: %s\r\nHost: %s\r\nConnection: %s\r\nSIZE: %s \r\n\r\n";

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
        sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0");

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
        	sprintf(query,client_msg,request_type,basename(filename),USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,file_size);

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
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0");
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
    	sprintf(query,client_msg,request_type,filename,USERAGENT,(_hostname+":"+_PORT).c_str(),CONNECTIONTYPE,"0");
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
void network_server::serve_forever() {
    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    int new_fd;
    char s[INET6_ADDRSTRLEN];
    int numbytes=0;
    char buf[MAXDATASIZE];
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    while(1) {
        sin_size=sizeof their_addr;
        new_fd=accept(_sockfd,(struct sockaddr*)&their_addr,&sin_size);
        if(new_fd==-1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr*)&their_addr),
                  s,sizeof s);
        if((numbytes=recv(new_fd,buf,MAXDATASIZE,0)) == -1) {
            perror("recv");
            continue;
        }
        char filename[30];
        char dummy[30];
        char request_type[30];
        char requested_file_size[30];
        sscanf(buf,client_msg,request_type,filename,dummy,dummy,dummy,requested_file_size);
        if(_lg) {
            _lg->add_write_log_task("Server recv "+string(request_type)+" type filename: "+filename +"from "+string(s));
        }

        char file[200];
        strcpy(file,homedir);
        strcat(file,"/dfs/");
        strcat(file,filename);
        // cout<<"filename: "<<file<<endl;

        if (!fork())
        {   // this is the child process
            close(_sockfd); // child doesn't need the listener
            int file_size=0;

            char info[30];
            char buf[BUFFER_SIZE];
            char response[BUFFER_SIZE];
            if(strcmp(request_type,"GET")==0) {
                std::ifstream fin(file,std::ios_base::in);
                if(fin.is_open()) {
                    char sizemsg[200];
                    strcpy(info,"200");
                    struct stat st;
                    stat(file,&st);
                    file_size=st.st_size;
                    sprintf(sizemsg, "%d", file_size);
                    int file_size_left=file_size;
                    sprintf(response, server_response_msg, info, filename,sizemsg);

                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                    while(fin) {
                        fin.read(buf,BUFFER_SIZE);
                        int cur_send=send(new_fd,buf,std::min(BUFFER_SIZE,file_size_left),0);
                        if(cur_send<0) {
                            perror("cannot send");
                            exit(1);
                        }
                        file_size_left-=cur_send;
                    }

                }
                else {
                    perror("cannot open the file...\n");
                    strcpy(info,"404");
                    sprintf(response, server_response_msg, info,filename,"0");
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                }
            }
            else if(strcmp(request_type,"POST")==0) {
            	cout<<"post branch"<<file<<endl;
                std::ofstream outfile(file);
                outfile.close();
                outfile.open(file, std::ios_base::app);
                if(outfile.is_open()) {
                    strcpy(info,"200");
                    sprintf(response, server_response_msg, info, filename,requested_file_size);
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                    int data_recv=0;
                    cout<<"requested_file_size: "<<requested_file_size<<endl;
                    int file_size_left=atoi(requested_file_size);
                    while(file_size_left>0) {
                        buf[0]='\0';
                        int cur_recv=recv(new_fd,buf,std::min(BUFFER_SIZE,file_size_left),0);
                        if(cur_recv<=0) {
                            perror("file_server_recv_post");
                            exit(1);
                        }
                        file_size_left-=cur_recv;
                        buf[cur_recv]='\0';
                        outfile<<buf;
                    }

                }
                else {
                    perror("cannot open the file...\n");
                    strcpy(info,"404");
                    char response[BUFFER_SIZE];
                    sprintf(response, server_response_msg, info,filename,"0");
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                }
                outfile.close();

            }
            else if(strcmp(request_type,"DELETE")==0) {
            	if(remove(file)==0){
            		_lg->add_write_log_task("File "+string(file)+" has been removed");
            		strcpy(info,"200");
            	}
            	else{
            		_lg->add_write_log_task("File "+string(file)+" doesnt exist");
            		strcpy(info,"404");
            	}
                sprintf(response, server_response_msg, info,filename,"0");
            	if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            }
            else if(strcmp(request_type,"LS")==0){
            	DIR *dir;
            	struct dirent* ent;
            	char path_name[100];
            	strcpy(path_name,homedir);
            	strcat(path_name,"/dfs/");
            	string folder_files="";
            	if((dir=opendir(path_name))!=nullptr){
            		while((ent=readdir(dir))!=nullptr){
            			string fname=string(ent->d_name);
            			if(fname!="." and fname !=".."){
            				folder_files+=string(ent->d_name);
            				folder_files+="\t";
            			}
            			
            		}
            		closedir(dir);
            		strcpy(info,"200");
            	}
            	else{
            		_lg->add_write_log_task("Fail to open folder ~/dfs");
            		strcpy(info,"200");
            	}
            	cout<<"folder file "<<folder_files<<endl;
            	sprintf(response, server_response_msg, info,filename,to_string(folder_files.size()).c_str());
            	if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            	if(send(new_fd,folder_files.c_str(),folder_files.size(),0)<0){
            		perror("cannot send");
            		exit(1);
            	}

            }
            // else if(strcmp(request_type,"STORE")==0){

            // }


            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }


}

void network::disconnect() {
    int fd=_sockfd;
    this->_sockfd=-1;
    this->_connected=false;
    close(fd);
}

bool network::send_msg(msg_t type) {
    if(send(_sockfd,&type,sizeof(msg_t),0)==-1) {
        perror("send error");
        return false;
    }
    return true;
}

int network::get_fd() {
    return _sockfd;
}
bool network::is_connected() {
    return _connected;
}
std::string network::hostname() {
    return _hostname;
}