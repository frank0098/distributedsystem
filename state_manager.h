#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H
#include "node.h"
#include "config.h"
#include <string>
#include <vector>
#include <mutex>

using std::string;
using std::vector;
class State_manager{
public:
	State_manager(){
		std::lock_guard<std::mutex> lg(_m);
		Config* conf=get_config();
		_size=conf->peer_ip.size();
		_self_id=conf->id;
		for(int i=0;i<_size;i++){
			Node node(i,conf->peer_ip[i],conf->election_server_port[i],conf->election_client_port[i],conf->file_server_port[i],conf->file_manager_port[i]);
			_peers.push_back(node);
		}
		_peers[_self_id].available=true;
		_leader_id=-1;
	}
	vector<Node> get_peers(){
		std::lock_guard<std::mutex> lg(_m);
		return _peers;
	}
	Node get_peer(int id){
		std::lock_guard<std::mutex> lg(_m);
		return _peers[id];
	}
	void set_alive(int id){
		std::lock_guard<std::mutex> lg(_m);
		_peers[id].available=true;		
	}
	void set_dead(int id){
		std::lock_guard<std::mutex> lg(_m);
		_peers[id].available=false;		
	}
	
	bool alive(int id){
		std::lock_guard<std::mutex> lg(_m);
		return _peers[id].available; 
	}
	int get_size(){
		return _size;
	}
	void set_leader(int id){
		if(id>=_peers.size()) return;
		std::lock_guard<std::mutex> lg(_m);
		_leader_id=id;
	}
	void remove_leader(){
		std::lock_guard<std::mutex> lg(_m);
		_leader_id=-1;		
	}
	bool get_leader(int &id){
		std::lock_guard<std::mutex> lg(_m);
		id=_leader_id;
		return _leader_id==-1;
	}
private:
	int _leader_id;
	int _self_id;
	int _size;
	std::mutex _m;
	vector<Node> _peers;
};

#endif