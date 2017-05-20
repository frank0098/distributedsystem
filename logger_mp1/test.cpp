#include "utility.h"
#include <assert.h>

void test1(vector<connection>& v,const char* server_test_log){
	const char* pattern="-e apple8";
	string matched_string_local=grep_server(pattern,server_test_log);
	grep_all(v,pattern);
	for(auto vv:v){
		if(vv.sockfd>=0){
			assert(matched_string_local==vv.content);
		}
	}
}

void test2(vector<connection>& v,const char* server_test_log){
	const char* pattern="-e apple";
	string matched_string_local=grep_server(pattern,server_test_log);
	grep_all(v,pattern);
	for(auto vv:v){
		if(vv.sockfd>=0){
			assert(matched_string_local==vv.content);
		}
	}
}

void test3(vector<connection>& v,const char* server_test_log){
	const char* pattern="-e GET";
	string matched_string_local=grep_server(pattern,server_test_log);
	grep_all(v,pattern);
	for(auto vv:v){
		if(vv.sockfd>=0){
			assert(matched_string_local==vv.content);
		}
	}
}

void read_and_send_file(vector<connection> v,istream &fin){

	char buffer[TEST_BUFFER_SIZE]; //reads only the first 1024 bytes
	msg_t msgtype=msg_t::client_write_test;
	long long buffer_sent=0;
	int cur_sent=0;
	vector<connection> nv;
	fin.seekg (0,fin.end);
	int size_left = fin.tellg();
	fin.seekg (0);
	for(auto vv:v){
		if(vv.sockfd>=0){
			nv.push_back(vv);
			if(send(vv.sockfd,&msgtype,sizeof(msgtype),0)==-1){
				perror("send type error");
			}
			if(send(vv.sockfd,&size_left,sizeof(int),0)==-1){
				perror("send size error");
			}
		}
	}
	while(size_left>0) {
		fin.read(buffer, sizeof(buffer));
	    for(auto vv:nv){
	    	if((cur_sent=send(vv.sockfd,buffer,fin.gcount(),0))==-1){
	    		perror("sned error during reading");
	    		exit(1);
	    	}
		}
		buffer_sent+=cur_sent;
		size_left-=cur_sent;
	}

	cout<<"test program sent "<<buffer_sent<<endl;
}
void test_func(){
	// TODO:READ LOCAL TEST FILE INTO A STRING AND THEN WRITE TO REMOTE FILE
	try{
        vector<connection> v;
        vector<string> ip_addrs=server_addr_read_config("server.cfg");
        for(auto ip:ip_addrs){
            v.push_back(connection{ip});
        }

        if(ip_addrs.empty()){
            perror("empty config file\n");
            return;
        }

		connect_all(v);
		struct passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;
		char path[50];
		strcpy(path,homedir);
		strcat(path,"/log");
		ifstream is(path);
		read_and_send_file(v,is);
		connect_all(v);
		test1(v,test_log_path);
		disconnect_all(v);
		connect_all(v);
		test2(v,test_log_path);
		disconnect_all(v);
		connect_all(v);
		test3(v,test_log_path);
		disconnect_all(v);



    }
    catch(std::runtime_error err){
        cout<<"runtime_error"<<err.what()<<endl;
    }
}


int main(){
	test_func();
	return 0;
}