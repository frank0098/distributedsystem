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
	vector<string> peer_ip;
	vector<string> peer_membership_port;
	vector<string> peer_election_port;
	vector<string> file_server_port;
	void load();
	Config();
private:
	std::string _path;
	bool _loaded;
};

static Config* get_config(){
	static Config instance;
	instance.load();
	return &instance;
}

#endif