#include "utility.h"
msg_t get_msg_type(int sockfd){
	cout<<"get msg type"<<endl;
	msg_t msgtype;
	if(recv(sockfd,&msgtype,sizeof (msgtype),0)<=0){
		perror("receive type fail");
		return msg_t::unknown;
	}
	return msgtype;

}
string utility_read(int sockfd){
	cout<<"utility_read"<<endl;
	string data;
	char buffer[BUFFER_SIZE];
	int read_bytes;
	if(recv(sockfd,&read_bytes,sizeof (int),0)<=0){
		perror("receive size fail");
		return "";
	}
	while(read_bytes>0){
		int cur_recv=recv(sockfd,buffer,BUFFER_SIZE-1,0);
		read_bytes-=cur_recv;
		if(cur_recv<=0){
			perror("error recv");
			break;
		}
		buffer[cur_recv]='\0';
		data+=string(buffer);
	}
	return data;
}

void utility_write_to_file(int sockfd){
	cout<<"utility_write_to_file"<<endl;
	string data;
	ofstream outfile(test_log_path);

	char buffer[TEST_BUFFER_SIZE];
	int read_bytes;
	if(recv(sockfd,&read_bytes,sizeof (int),0)<=0){
		perror("receive size fail");
	}
	while(read_bytes>0){
		int cur_recv=recv(sockfd,buffer,TEST_BUFFER_SIZE-1,0);
		read_bytes-=cur_recv;
		if(cur_recv<=0){
			perror("error recv");
			break;
		}
		outfile.write(buffer,cur_recv);
	}
	outfile.close();
}
bool utility_send(int sockfd,const char* msg,msg_t msgtype){
	// cout<<"sending "<<msg<<endl;
	// cout<<"len "<<strlen(cmd)<<endl;
	if(send(sockfd,&msgtype,sizeof(msgtype),0)==-1){
		perror("send error");
		return false;
	}
	int msg_length=strlen(msg);
	int data_sent=0;
	if(send(sockfd,&msg_length,sizeof(int),0)==-1){
		perror("send error");
		return false;
	}
	// cout<<"send length "<<msg_length<<endl;
	
    while(data_sent<msg_length){
    	int sending=0;
    	if((sending=send(sockfd,msg+data_sent,min(BUFFER_SIZE-1,msg_length-data_sent),0))==-1){
	        perror("send error\n");
	        return false;
	    }
	    data_sent+=sending;
    }
    return true;
}

void serve_forever(int sockfd){

	struct sockaddr_storage their_addr;
	int new_fd;
	char s[INET6_ADDRSTRLEN];
	socklen_t sin_size;
	while(1){
		sin_size=sizeof(their_addr);
		new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);
		if(new_fd==-1){
			perror("accept");
			continue;
		}
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s,sizeof(s));
		cout<<"server:got connection from"<<s<<endl;
		msg_t msg_type=get_msg_type(new_fd);
		if(msg_type==msg_t::client_grep){
			string query=utility_read(new_fd);
			const string grep_result=grep_server(query.c_str(),log_path_read_config("logpath.cfg").c_str());
			if(!fork()){
				close(sockfd);
				const char* thing_to_send = grep_result.c_str();
				if(!utility_send(new_fd,thing_to_send,msg_t::server_result)){
					perror("send fail??");
				}
				cout<<"msg sent completed"<<endl;
				close(new_fd);
				exit(0);
			}
		}
		else if(msg_type==msg_t::client_write_test){
			utility_write_to_file(new_fd);
		}
		else{
			cout<<"server: receive wrong msg type"<<endl;
		}

		close(new_fd);
	}
}


string log_path_read_config(string config_file_path){
	// cout<<config_file_path<<endl;
	std::ifstream in(config_file_path);
	string log_path;
	if(!(in>>log_path)){
		throw std::runtime_error("invalid input");
	}
	if(log_path.empty()){
		throw std::runtime_error("no cfg provided");
	}
	return log_path;
}


vector<string> server_addr_read_config(string config_file_path){
	std::ifstream in(config_file_path);
	string ip_addr;
	std::vector<string> v;
	struct sockaddr_in sa;
	while(in>>ip_addr){
		const char* tmp_addr=ip_addr.c_str(); 
		if(inet_pton(AF_INET,tmp_addr,&(sa.sin_addr))==0 &&
		 inet_pton(AF_INET6,tmp_addr,&(sa.sin_addr))==0){
			throw std::runtime_error("invalid ip_addr: "+ip_addr);
		}
		else{
			v.push_back(ip_addr);
		}
	}
	return v;
}


string grep_server(const char* cmd,const char* log_path){
	array<char,128> buffer;
	string result;
	char cmd_buf[BUFFER_SIZE];
	strcpy(cmd_buf,"grep ");
	strcat(cmd_buf,log_path);
	strcat(cmd_buf," ");
	strcat(cmd_buf,cmd);
	shared_ptr<FILE> pipe(popen(cmd_buf,"r"),pclose);
	if(!pipe) throw std::runtime_error("popen() failed!");
	while(!feof(pipe.get())){
		if(fgets(buffer.data(),128,pipe.get())!=NULL){
			result+=buffer.data();
		}
	}
	return result;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_connection(const char* ip_addr){
	int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip_addr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
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
        return -1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    return sockfd;
}

int server_setup_connection(){
	int sockfd,yes=1,rv;
	struct addrinfo hints,*servinfo,*p;
	
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
	return sockfd;
}

string distrbuted_grep(string ip_addr,int sockfd,const char* cmd){
	string res;
    if(sockfd==-1){
        perror("socket error\n");
        return "";
    }
    if(!utility_send(sockfd,cmd,msg_t::client_grep)) return "";
    printf("\n ==========client:received from %s =============\n",ip_addr.c_str());
    if(get_msg_type(sockfd)==msg_t::server_result){
    	res=utility_read(sockfd);
    }
	return res;
}

void connect_all(vector<connection>& v){
	for(auto &vv:v){
		vv.sockfd=get_connection(vv.hostname.c_str());
	}
}
void disconnect_all(vector<connection> v){
	for(auto vv:v){
		close(vv.sockfd);
	}	
}
void grep_all(vector<connection>& v,const char* cmd){
	for(auto &vv:v){
		vv.content=distrbuted_grep(vv.hostname,vv.sockfd,cmd);
	}
}
void print_all(vector<connection> v){
	for(auto vv:v){
		cout<<"receive grep from "<<vv.hostname<<endl;
		cout<<vv.content<<endl;
	}
}

// FOR Testing
// string read_file(istream &is,const char* file_path){

// }

ostream& write_file(ostream &os,const char* file_path,string content){
	// os.open(file_path);
	os<<content;
	return os;
}

