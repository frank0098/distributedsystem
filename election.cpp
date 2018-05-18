#include "election.h"

Election_manager::Election_manager(State_manager* sm):_election_in_process(false),_running(true),_nw_server(nullptr),_nw_client(nullptr),_sm(sm),_membership(nullptr){
		assert(sm);
		_membership=new Membership(sm,this);
		_membership->start();

		Config* conf=get_config();
		_id=conf->id;
		_highest_id=conf->peer_ip.size()-1;
		strcpy(_hostname,conf->peer_ip[_id].c_str());
		strcpy(_m_election_server_port,conf->election_server_port[_id].c_str());
		strcpy(_m_election_client_port,conf->election_client_port[_id].c_str());

		_nw_server=new Network_UDP(_hostname,_m_election_server_port);
		_nw_client=new Network_UDP(_hostname,_m_election_client_port);
		_nw_server->connect();
		_nw_client->connect();


		_election_thread=std::thread([&](){
			this->election_listner();
		});
		_election_client_thread=std::thread([&]{
			this->run_election();
		});

		_sm->set_alive(_id);
		_sm->remove_leader();
		start_election();
}
Election_manager::~Election_manager(){

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
	if(_membership){
		delete _membership;
	}
}
void Election_manager::stop(){
	_membership->stop();
	_running=false;
	_cv.notify_one();
	_nw_server->disconnect();
	_nw_client->disconnect();
}
void Election_manager::start_election(){
	std::lock_guard<std::mutex> lg(_m);
	if(!_election_in_process){
		_election_in_process=true;
		_cv.notify_one();
	}	
}
void Election_manager::run_election(){

	while(_running){
		{
			std::unique_lock<std::mutex> l(_m);
			_cv.wait(l,[this]{return !_running||_election_in_process;});
		}
		if(!_running) break;

		logger()->write(" ELECTION START");

		vector<Node> peers=_sm->get_peers();
		if(_id==_highest_id){
			logger()->write("ELECTION node with highest id");
			for(auto &x:peers){
				if(x.peer_id!=_id && x.available){
					Network_UDP::send_message(msg_t::COORDINATOR,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,std::to_string(x.election_client_port).c_str(),x.peer_ip.c_str(),std::to_string(_id).c_str());
				}
			}
			_sm->set_leader(_id);
			{
				std::lock_guard<std::mutex> lg(_m);
				_election_in_process=false;
			}

		}
		else{
			logger()->write("ELECTION send message to nodes with higher id");
			vector<Peer_struct> v;
			for(auto &x:peers){
				Peer_struct ps(x.peer_id,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str());
				v.push_back(ps);
				if(x.peer_id>_id){
					Network_UDP::send_message(msg_t::ELECTION,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,_m_election_client_port,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str());
				}
			}
			_nw_client->wait_message_from_peers(v);
			if(!_running) break;
			bool receive_something=false;
			for(auto &x:v){
				if(x.suspicious==false){
					receive_something=true;
				}
			}
			if(!receive_something){
				logger()->write("ELECTION: higher nodes are unavaialbe. set self leader.");
				_sm->set_leader(_id);
				for(auto &x:peers){
					cout<<"election"<<x.peer_id<<" "<<x.available<<endl;
				if(x.peer_id<_id && x.available){
					
					logger()->write("ELECTION: SEND TO NODE"+std::to_string(x.peer_id));
					Network_UDP::send_message(msg_t::COORDINATOR,x.peer_ip.c_str(),std::to_string(x.election_server_port).c_str(),_hostname,_m_election_client_port,x.peer_ip.c_str(),std::to_string(_id).c_str());
					}
				}
				{
					std::lock_guard<std::mutex> lg(_m);
					_election_in_process=false;
				}
			}
			else{
				logger()->write("ELECTION: received something. start election agian");
				sleep(SLEEP_INTERVAL);
			}
		}
		logger()->write(" ELECTION FINISHED");
	}
}

void Election_manager::election_listner(){
	char source[IP_LENGTH];
	char source_port[PORT_LENGTH];
	char info[IP_LENGTH];
	char info2[PORT_LENGTH];

	string str;
	logger()->write("ELECTION: starts");
	while(_running){
		msg_t msgtype=_nw_server->recv_message(source,source_port,info,info2);
		switch(msgtype){
			case msg_t::COORDINATOR:

			logger()->write("ELECTION: received COORDINATOR");
			{
				int newleaderid=atoi(info2);
				if(newleaderid>_highest_id){
					logger()->write("FATAL impossible id"+std::to_string(newleaderid));
				}
				else{
					_sm->set_leader(newleaderid);
					{
						std::lock_guard<std::mutex> lg(_m);
						_election_in_process=false;
					}
					Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_election_server_port,info,info2);
				}
			}
			break;
			case msg_t::ELECTION:

			logger()->write("ELECTION: received ELECTION");
			start_election();
			Network_UDP::send_message(msg_t::ACK,source,source_port,_hostname,_m_election_server_port,info,info2);
			break;
			case msg_t::UNKNOWN:
			break;
			default:
			logger()->write("ELECTION: received election");		
			break;
		}
	}
	logger()->write("ELECTION: finished");
}

