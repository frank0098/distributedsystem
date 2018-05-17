#ifndef ELECTION_H
#define ELECTION_H

#include "node.h"
#include "config.h"
#include "logger.h"
#include "state_manager.h"
#include "network_manager.h"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <assert.h>

class Election{

public:
	Election(State_manager* sm):_election_in_process(false),_running(true),_nw_server(nullptr),_nw_client(nullptr),_sm(sm){
		assert(sm);
		Config* conf=get_config();
		_id=conf->id;
		_highest_id=conf->peer_ip.size()-1;
		strcpy(_hostname,conf->peer_ip[_id].c_str());
		strcpy(_m_election_server_port,conf->election_server_port[_id].c_str());
		strcpy(_m_election_client_port,conf->election_client_port[_id].c_str());
		_election_thread=std::thread([&](){
			this->election_listner();
		});
		_nw_server=new Network_UDP(_hostname,_m_election_server_port);
		_nw_client=new Network_UDP(_hostname,_m_election_client_port);
		_nw_server->connect();
		_nw_client->connect();
		start_elction();
	}
	~Election(){
		if(_election_thread.joinable()){
			_election_thread.join();
		}
		if(_election_client_thread.joinable()){
			_election_client_thread.join();
		}
		if(_sm){
			delete _sm;
		}
		if(_nw_server){
			delete _nw_server;
		}
		if(_nw_client){
			delete _nw_client;
		}
	}
	void stop(){
		_running=false;
		_nw_server->disconnect();
		_nw_client->disconnect();
	}
	void start_elction(){
		if(!_running) return;
		vector<Node> peers=_sm->get_peers();
		if(_id==_highest_id){
			for(auto &x:peers){
				if(x.peer_id!=_id && x.available){
					Network_UDP::send_message(msg_t::COORDINATOR,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,std::to_string(x.election_client_port).c_str(),x.peer_ip.c_str(),std::to_string(_id).c_str());
				}
			}
			_sm->set_leader(_id);

		}
		else{
			vector<Peer_struct> v;
			for(auto &x:peers){
				Peer_struct ps(x.peer_id,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str());
				v.push_back(ps);
				if(x.peer_id>_id){
					Network_UDP::send_message(msg_t::ELECTION,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,_m_election_client_port,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str());
				}
			}
			_nw_client->wait_message_from_peers(v);
			bool receive_something=false;
			for(auto &x:v){
				if(x.suspicious==false){
					receive_something=true;
				}
			}
			if(!receive_something){
				_sm->set_leader(_id);
				for(auto &x:peers){
				if(x.peer_id!=_id && x.available){
					Network_UDP::send_message(msg_t::COORDINATOR,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,_m_election_client_port,x.peer_ip.c_str(),std::to_string(_id).c_str());
					}
				}
			}
			else{
				sleep(SLEEP_INTERVAL);
				start_elction();
			}
		}
		_election_in_process=false;

	}
private:
	void election_listner(){
		char source[IP_LENGTH];
		char source_port[PORT_LENGTH];
		char info[IP_LENGTH];
		char info2[PORT_LENGTH];
		logger()->write("election_listner starts");
		while(_running){
			msg_t msgtype=_nw_server->recv_message(source,source_port,info,info2);
			switch(msgtype){
				case msg_t::COORDINATOR:
				{
					_election_in_process=false;
					int newleaderid=atoi(info2);
					if(newleaderid>_highest_id){
						logger()->write("FATAL impossible id"+std::to_string(newleaderid));
						_sm->set_leader(newleaderid);
						Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_election_server_port,info,info2);
					}
				}
				break;
				case msg_t::ELECTION:
				if(!_election_in_process){
					_election_in_process=true;
					_sm->remove_leader();
					_election_client_thread=std::thread([&](){
						start_elction();
					});
				}
				break;
				default:
				logger()->write("UNEXPECTED MESSAGE");
				break;
			}
		}
		logger()->write("election_listner finished");
	}
	bool _running;
	bool _election_in_process;

	std::thread _election_thread;
	std::thread _election_client_thread;
	State_manager* _sm;
	Network_UDP* _nw_server;
	Network_UDP* _nw_client;

	unsigned int _id;
	unsigned int _highest_id;
	char _hostname[IP_LENGTH];
	char _m_election_server_port[PORT_LENGTH];
	char _m_election_client_port[PORT_LENGTH];


};

#endif