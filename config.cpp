#include "config.h"


static Config instance;

Config* get_config(){
	instance.load();
	return &instance;
}

static bool contains(std::string needle,std::string &stack){
	if(stack.substr(0,needle.size())==needle) return true;
	return false;
}
static void parse(string str,vector<string>& v){
	int i=0,j=0;
	while(i<str.size() and j<str.size()){
		while(str[i]==' ')i++;
		j=i;
		while(str[j]!=';')j++;
		v.push_back(str.substr(i,j-i));
		i=j+1;
	}
}
Config::Config(){
	_path="./conf.cfg";
	_loaded=false;
}
void Config::load(){
	// std::cout<<"la"<<std::endl;
	if(_loaded) return;

	std::ifstream f;
	f.open(_path);
	std::string line;
	while(std::getline(f,line)){
		if(contains("peer_ip",line)){
			if(std::getline(f,line)){
				parse(line,peer_ip);
			}
		}
		else if(contains("peer_membership_server_port",line)){
			if(std::getline(f,line)){
				parse(line,peer_membership_server_port);
			}
		}
		else if(contains("peer_membership_client_port",line)){
			if(std::getline(f,line)){
				parse(line,peer_membership_client_port);
			}
		}
		else if(contains("election_server_port",line)){
			if(std::getline(f,line)){
				parse(line,election_server_port);
			}
		}
		else if(contains("election_client_port",line)){
			if(std::getline(f,line)){
				parse(line,election_client_port);
			}
		}
		else if(contains("file_server_port",line)){
			if(std::getline(f,line)){
				parse(line,file_server_port);
			}
		}
		else if(contains("fs_path",line)){
			if(std::getline(f,line)){
				fs_path=line;
			}
		}

	}
	_loaded=true;
}