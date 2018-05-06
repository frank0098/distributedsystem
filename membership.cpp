#include "membership.h"
void Membership::start(){
	// memset(peer_map,false,sizeof(bool)*MAX_MEMBERS);
	std::cout<<"memstart"<<std::endl;
	//load conf
	Config* conf=get_config();
	_id=conf->id;
	if(_id>=MAX_MEMBERS or _id<0){
		logger()->write("invalid id");
		exit(EXIT_FAILURE);
	}

	strcpy(_hostname,conf->peer_ip[_id].c_str());
	strcpy(_m_server_port,conf->peer_membership_server_port[_id].c_str());
	strcpy(_m_client_port,conf->peer_membership_client_port[_id].c_str());


	//start network module
	_nw_server=new Network_UDP(_hostname,_m_server_port);
	_nw_client=new Network_UDP(_hostname,_m_client_port);
	_nw_server->connect();
	_nw_client->connect();
	//start workers
	_worker_send_message=std::thread([&](){
		send_message_worker();
	});
	_worker_server=std::thread([&](){
		run_membership_server();
	});
	_worker_client=std::thread([&](){
		run_membership_client();
	});
	//send join message to peers
	int size=conf->peer_ip.size();;
	vector<Peer_struct> v;
	_q.block();
	for(int i=0;i<size;i++){
		if(i==_id) continue;
		Peer_struct p;
		Work_item item;
		p.id=i;
		strcpy(p.peerip,conf->peer_ip[i].c_str());
		strcpy(p.peerport,conf->peer_membership_client_port[i].c_str());
		p.suspicious=true;
		strcpy(item.source_ip,_hostname);
		strcpy(item.source_port,_m_client_port);
		strcpy(item.dest_ip,p.peerip);
		strcpy(item.dest_port,p.peerport);
		strcpy(item.additional_ip,std::to_string(_id).c_str());
		strcpy(item.additional_port,std::to_string(_id).c_str());
		item.type=msg_t::JOIN;
		v.push_back(p);
		_q.push(item);

	}
	_q.unblock();
	_nw_client->wait_message_from_peers(v);
	std::lock_guard<std::mutex> lg(_m);
	for(auto &p:v){
		if(!p.suspicious){
			_peers_v.push_back(p);
			// peer_map[p.id]=true;
		}
	}
}
void Membership::stop(){
	vector<Peer_struct> v;
	{
		std::lock_guard<std::mutex> lg(_m);
		v=_peers_v;
	}
	for(auto &p:v){
		Network_UDP::send_message(msg_t::EXIT,p.peerip,p.peerport,_hostname,_m_server_port,_hostname,_m_server_port);
	}
	_q.stop();
	_running=false;
	_nw_server->disconnect();
	_nw_client->disconnect();
	delete _nw_server;
	delete _nw_client;
}
//this thread runs like an echo server
void Membership::run_membership_server(){
	char source[IP_LENGTH];
	char source_port[PORT_LENGTH];
	char info[IP_LENGTH];
	char info_port[PORT_LENGTH];
	while(_running){
		msg_t msgtype=_nw_server->recv_message(source,source_port,info,info_port);
		msg_t ret_msgtype=msg_t::UNKNOWN;
		switch(msgtype){
			case msg_t::JOIN:
			add_member(source,source_port,std::stoi(info));
			if(!Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_server_port,info,info_port)){
				logger()->write("FATAL:UNABLE TO SEND, PING");
			}
			break;

			case msg_t::EXIT:
			remove_member(std::stoi(info));
			break;
			case msg_t::PING:
			if(!Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_server_port,info,info_port)){
				logger()->write("FATAL:UNABLE TO SEND, PING");
			}
			break;

			case msg_t::INDIRECT_PING:
			if(!Network_UDP::send_message(msg_t::PING,info,info_port,_hostname,_m_server_port,source,source_port)){
				logger()->write("FATAL:UNABLE TO SEND,INDIRECT_PING");
			}
			break;

			case msg_t::ACK://only happen after receive indirect ping
			if(!Network_UDP::send_message(msg_t::ACK,info,info_port,_hostname,_m_server_port,info,info_port)){
				logger()->write("FATAL:UNABLE TO SEND,ACK");
			}
			break;

			default:
			break;
		}
	}
}
//this thread send message every few seconds to see if peer still alive
void Membership::run_membership_client(){
	while(_running){

		sleep(SLEEP_INTERVAL);
		while(!_q.empty()){

		}
		_q.block();
		for(auto &p:_peers_v){
			p.suspicious=true;
			Work_item item;
			strcpy(item.source_ip,_hostname);
			strcpy(item.source_port,_m_client_port);
			strcpy(item.dest_ip,p.peerip);
			strcpy(item.dest_port,p.peerport);
			strcpy(item.additional_ip,_hostname);
			strcpy(item.additional_port,_m_client_port);
			item.type=msg_t::PING;
			_q.push(item);
		}
		_q.unblock();
		vector<Peer_struct> peers_copy;
		{
			std::lock_guard<std::mutex> lg(_m);
			peers_copy=_peers_v;
		}
		_nw_client->wait_message_from_peers(peers_copy);
		vector<Peer_struct> suspicious_peers;
		vector<Peer_struct> not_suspicious_peers;
		for(auto &p:peers_copy){
			if(p.suspicious){
				suspicious_peers.push_back(p);
			}
			else{
				not_suspicious_peers.push_back(p);
			}
		}
		if(suspicious_peers.size()>0){
			vector<Peer_struct> indirect_ping_peers;
			for(auto &p:suspicious_peers){
				srand (time(NULL));
				int idx = rand() % not_suspicious_peers.size();
				indirect_ping_peers.push_back(not_suspicious_peers[idx]);
				indirect_ping_peers[0].suspicious=true;
				_nw_client->wait_message_from_peers(indirect_ping_peers);
				if(indirect_ping_peers[0].suspicious){
					remove_member(indirect_ping_peers[0].id);
				}
			}
		}

	}
}
void Membership::add_member(const char* source,const char* port,int id){
	for(auto &p:_peers_v){
		if(p.id==id) return;
	}
	Peer_struct p;
	p.id=id;
	strcpy(p.peerip,source);
	strcpy(p.peerport,port);
	std::lock_guard<std::mutex> lg(_m);
	_peers_v.push_back(p);


}
void Membership::remove_member(int id){
	int idx=-1;
	for(int i=0;i<_peers_v.size();i++){
		if(_peers_v[i].id==id){
			idx=i;
			break;
		}
	}
	if(idx<0) return;
	std::lock_guard<std::mutex> lg(_m);
	_peers_v.erase(_peers_v.begin()+idx);
}
//worker to process send message
void Membership::send_message_worker(){
	while(_running){
		Work_item item;
		_q.wait_and_pop(item);
		if(!Network_UDP::send_message(item.type,item.dest_ip,item.dest_port,item.source_ip,item.source_port,item.additional_ip,item.additional_port)){
			logger()->write("FATAL:UNABLE TO SEND,worker");
		}
	}
}



