#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>

using namespace std;

/*
Function that read config and initialize server list
*/
void server_addr_read_config();
/*
Function that read config to initialize log path
*/
void log_path_read_config();
/*
Function that accepts a query and send it to client
*/
void grep_client(string query);
/*
Function accepts a params string and generate a string output
*/
string grep_server(const char* cmd);

