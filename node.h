#ifndef NODE_H
#define NODE_H

#include <string>
using std::string;
class Node{
public:
	bool available;
	unsigned int peer_id;
	string peer_ip;
	// unsigned short membership_port;
	unsigned short election_server_port;
	unsigned short election_client_port;
	unsigned short file_server_port;
	unsigned short file_manager_port;
	// MSGPACK_DEFINE(peer_id,peer_ip,election_port,file_server_port);
	Node(unsigned int peer_id_,string peer_ip_,string election_server_port_,string election_client_port_,string file_server_port_,string file_manager_port_){
		peer_id=peer_id_;
		peer_ip=peer_ip_;
		election_server_port=std::stoi(election_server_port_);
		election_client_port=std::stoi(election_client_port_);
		file_server_port=std::stoi(file_server_port_);
		file_manager_port=std::stoi(file_manager_port_);
		available=false;
	}
	Node(const Node& other){
		copy(other);
	}
	Node& operator=(const Node& other){
		if(this!=&other){
			copy(other);
		}
		return *this;
	}
	void copy(const Node& other){
		peer_id=other.peer_id;
		peer_ip=other.peer_ip;
		election_server_port=other.election_server_port;
		election_client_port=other.election_client_port;
		file_server_port=other.file_server_port;
		file_manager_port=other.file_manager_port;
	}
};

#endif