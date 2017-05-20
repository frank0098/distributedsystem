#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

using namespace std;
#define BUFFER_SIZE 200
#define TEST_BUFFER_SIZE 20000
#define MAXDATASIZE 2000000 // max number of bytes we can get at once 
#define PORT "3490" // the port client will be connecting to 
// const char* test_log_path="test.log";
#define test_log_path "test.log"
enum msg_t {
	unknown=0,
	server_result=1,
	server_msg=2,
	client_grep=3,
	client_write_test=4,
};
struct connection{
	string hostname;
	int sockfd;
	string content;
	connection(string h):hostname(h),sockfd(-1),content(""){

	}
};
/*
Function that read config and initialize server list
*/
vector<string> server_addr_read_config(string config_file_path);
/*
Function that read config to initialize log path
*/
string log_path_read_config(string config_file_path);
/*
Function that accepts a query and send it to client
*/
void grep_client(string query);
/*
Function accepts a params string and generate a string output
*/
string grep_server(const char* cmd,const char* log_path);

int get_connection(const char* ip);

void *get_in_addr(struct sockaddr *sa);

int server_setup_connection();

void connect_all(vector<connection>& v);

void print_all(vector<connection> v);

void serve_forever(int sockfd);

void grep_all(vector<connection>& v,const char* cmd);

void disconnect_all(vector<connection> v);

#endif
