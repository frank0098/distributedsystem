#ifndef NODE_H
#define NODE_H

#include <string>
using std::string;
class Node{
public:
	unsigned int peer_id;
	string peer_ip;
	unsigned short membership_port;
	unsigned short election_port;
	unsigned short file_server_port;
	MSGPACK_DEFINE(peer_id,peer_ip,membership_port,election_port,file_server_port)
	Node(unsigned int peerid_,string peer_ip_,string membership_port_,string election_port_,string file_server_port_){
		peer_id=peer_id_;
		peer_ip=peer_ip_;
		membership_port=std::stoi(membership_port_);
		election_port=std::stoi(election_port_);
		file_server_port=std::stoi(file_server_port_);
	}
	Node(const Node& other){
		peer_id=other.peer_id;
		peer_ip=other.peer_ip;
		membership_port=other.membership_port;
		election_port=other.election_port;
		file_server_port=other.file_server_port;
	}
	Node& operator=(const Node& other){
		if(this!=&other){
			peer_id=other.peer_id;
			peer_ip=other.peer_ip;
			membership_port=other.membership_port;
			election_port=other.election_port;
			file_server_port=other.file_server_port;
		}
		return *this;
	}
};

#endif