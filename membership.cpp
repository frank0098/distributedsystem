#include "membership.h"

Membership::~Membership(){

	if(_worker_server.joinable()){
		_worker_server.join();
	}
	if(_worker_client.joinable()){
		_worker_client.join();
	}
	_q.stop();
	if(_worker_send_message.joinable()){
		_worker_send_message.join();
	}
}
void Membership::start(){
	//load conf
	Config* conf=get_config();
	_id=conf->id;
	if(_id>=MAX_MEMBERS or _id<0){
		logger()->write("invalid id");
		exit(EXIT_FAILURE);
	}
	if(conf->peer_membership_server_port.size()!=conf->peer_membership_client_port.size() || conf->peer_membership_server_port.size()!=conf->peer_ip.size() || _id>=conf->peer_ip.size()){
		logger()->write("invalid conf");
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
	_running=true;
	_worker_send_message=std::thread([&](){
		this->send_message_worker();
	});
	_worker_server=std::thread([&](){
		this->run_membership_server();
	});

	//send join message to peers
	int size=conf->peer_ip.size();;
	vector<Peer_struct> v;
	
	_q.block();
	for(int i=0;i<size;i++){
		if(i==_id) continue;
		Peer_struct p;
		
		p.id=i;
		strcpy(p.peerip,conf->peer_ip[i].c_str());
		strcpy(p.peerport,conf->peer_membership_server_port[i].c_str());
		p.suspicious=true;
		Work_item item(msg_t::JOIN,p.peerip,p.peerport,_hostname,_m_client_port,std::to_string(_id).c_str(),_m_server_port);
		v.push_back(p);
		_q.push(item);

	}

	_q.unblock();
	_nw_client->wait_message_from_peers(v);
	for(auto &p:v){
		if(!p.suspicious){
			add_member(p.peerip,p.peerport,p.id);
		}
	}
	cout<<"start phase completed"<<endl;
	_worker_client=std::thread([&](){
		this->run_membership_client();
	});
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
	_q.block();
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
			case msg_t::JOIN:{
				add_member(source,info_port,std::stoi(info));
				Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_server_port,info,info_port);

			}
			break;
			case msg_t::EXIT:{
				remove_member(std::stoi(info));
			}
			break;
			case msg_t::PING:{
				Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_server_port,info,info_port);
			}
			break;
			case msg_t::INDIRECT_PING:{
				Network_UDP::send_message(msg_t::PING,info,info_port,_hostname,_m_server_port,source,source_port);
			}
			break;
			case msg_t::ACK:{//only happen after receive indirect ping
				Network_UDP::send_message(msg_t::ACK,info,info_port,_hostname,_m_server_port,info,info_port);
			}
			break;

			default:
			break;
		}
	}
	cout<<"end1"<<endl;
}
//this thread send message every few seconds to see if peer still alive
void Membership::run_membership_client(){
	while(true){
		// cout<<"1"<<endl;
		sleep(SLEEP_INTERVAL);
		if(!_running) break;
		while(!_q.empty()){
			cout<<"runrunrunrun"<<endl;
			cout<<_q.size()<<endl;
		}
		// cout<<"2"<<endl;
		string outputstr=" members: ";
		for(auto &p:_peers_v){
			outputstr+="id:";
			outputstr+=std::to_string(p.id);
			outputstr+=" ";
			outputstr+=p.peerip;
			outputstr+=":";
			outputstr+=p.peerport;
			outputstr+="; ";
		}
		// cout<<"3"<<endl;
		logger()->write(outputstr);
		_q.block();
		for(auto &p:_peers_v){
			p.suspicious=true;
			Work_item item(msg_t::PING,p.peerip,p.peerport,_hostname,_m_client_port,_hostname,_m_client_port);
			// cout<<"enqueue"<<endl;
			// item.print();
			_q.push(item);
		}
		_q.unblock();
		vector<Peer_struct> peers_copy;
		{
			std::lock_guard<std::mutex> lg(_m);
			peers_copy=_peers_v;
		}
		cout<<"4"<<endl;
		_nw_client->wait_message_from_peers(peers_copy);
		cout<<"5"<<endl;
		// cout<<peers_copy.size()<<endl;
		vector<Peer_struct> suspicious_peers;
		vector<Peer_struct> not_suspicious_peers;
		for(auto &p:peers_copy){
			if(p.suspicious){
				logger()->write("suspicious "+std::to_string(p.id));
				suspicious_peers.push_back(p);
			}
			else{
				not_suspicious_peers.push_back(p);
			}
		}
		logger()->write("suspicious member cnt: "+std::to_string(suspicious_peers.size()));
		// cout<<suspicious_peers.size()<<endl;
		if(suspicious_peers.size()>0){
			
			if(not_suspicious_peers.size()==0){//this should not happen given the assumption that more than half of the members are alive
				remove_member(suspicious_peers[0].id);
				continue;
			}
			for(auto &p:suspicious_peers){
				vector<Peer_struct> indirect_ping_peers;
				srand (time(NULL));
				int idx = rand() % not_suspicious_peers.size();
				indirect_ping_peers.push_back(not_suspicious_peers[idx]);
				indirect_ping_peers[0].suspicious=true;

				Peer_struct& notp=not_suspicious_peers[idx];
				Network_UDP::send_message(msg_t::INDIRECT_PING,notp.peerip,notp.peerport,_hostname,_m_client_port,p.peerip,p.peerport);
				_nw_client->wait_message_from_peers(indirect_ping_peers);
				if(indirect_ping_peers[0].suspicious){
					remove_member(p.id);
				}
			}
		}

	}
	cout<<"end2"<<endl;
}
void Membership::add_member(const char* source,const char* port,int id){
	// cout<<"addmembercalled"<<source<<port<<id<<endl;
	for(auto &p:_peers_v){
		string sourcestr(source);
		string portstr(port);
		logger()->write(" add member: ip"+sourcestr+" "+portstr+" id"+std::to_string(id));
		if(p.id==id) return;
	}
	Peer_struct p;
	p.id=id;
	strcpy(p.peerip,source);
	strcpy(p.peerport,port);
	{
		std::lock_guard<std::mutex> lg(_m);
		_peers_v.push_back(p);
	}
	string outputstr="membership: successfully add ";
	outputstr+=p.peerip;
	outputstr+=" ";
	outputstr+=p.peerport;
	outputstr+=" id: ";
	outputstr+=std::to_string(id);
	logger()->write(outputstr);


}
void Membership::remove_member(int id){
	// cout<<"remove"<<id<<endl;
	int idx=-1;
	for(int i=0;i<_peers_v.size();i++){
		if(_peers_v[i].id==id){
			idx=i;
			break;
		}
	}
	if(idx<0) return;
	Peer_struct& p=_peers_v[idx];
	string outputstr="membership: successfully remove ";
	outputstr+=p.peerip;
	outputstr+=" ";
	outputstr+=p.peerport;
	outputstr+=" id: ";
	outputstr+=std::to_string(p.id);
	// cout<<"removeagain"<<idx<<endl;
	{
	std::lock_guard<std::mutex> lg(_m);
	_peers_v.erase(_peers_v.begin()+idx);
	}

	logger()->write(outputstr);

	// cout<<"remove finished"<<idx<<endl;
}
//worker to process send message
void Membership::send_message_worker(){
	while(_running){
		Work_item item;
		_q.wait_and_pop(item);
		if(!_running) break;
		// cout<<"sending: ";item.print();
		if(!Network_UDP::send_message(item.type,item.dest_ip,item.dest_port,item.source_ip,item.source_port,item.additional_ip,item.additional_port)){
			logger()->write("err:UNABLE TO SEND,worker");
		}
	}
	cout<<"end3"<<endl;
}



