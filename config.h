#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <vector>


using std::vector;
using std::string;
class Config{
public:
	int id;
	vector<string> peer_ip;
	vector<string> peer_membership_server_port;
	vector<string> peer_membership_client_port;
	vector<string> election_server_port;
	vector<string> election_client_port;
	vector<string> file_server_port;
	string fs_path;
	void load();
	Config();
private:
	std::string _path;
	bool _loaded;
};

Config* get_config();

#endif