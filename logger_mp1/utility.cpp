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
	char cmd_buf[CMD_BUFFER_SIZE];
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