#include "utility.h"


string log_path_read_config(string config_file_path){
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
	strcat(cmd_buf,log_path_read_config(log_path).c_str());
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