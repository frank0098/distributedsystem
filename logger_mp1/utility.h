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


using namespace std;
#define BUFFER_SIZE 200
#define PORT "3490" // the port client will be connecting to 
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

