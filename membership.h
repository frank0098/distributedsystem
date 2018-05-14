#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include "network_manager.h"
#include "state_manager.h"
#include "logger.h"
#include "config.h"


#include <string>
#include <vector>
#include <mutex>
#include <thread>

#define MAX_MEMBERS 10
using std::cout;
using std::endl;
//SWIM Infection-style Dissemination
class Membership{
public:
	Membership(State_manager* sm);
	void start();
	void stop();
	~Membership();
private:
	void run_membership_server();
	void run_membership_client();
	void send_message_worker();
	void add_member(const char* source,const char* port,int id);
	void remove_member(int id);
	char _hostname[IP_LENGTH];
	char _m_server_port[PORT_LENGTH];
	char _m_client_port[PORT_LENGTH];

	vector<Peer_struct> _peers_v;
	std::mutex _m;
	Network_UDP* _nw_server;
	Network_UDP* _nw_client;
	State_manager* _sm;
	Queue<Work_item> _q;
	std::thread _worker_send_message;
	std::thread _worker_server;
	std::thread _worker_client;
	// bool peer_map[MAX_MEMBERS];
	
	int _id;
	bool _running;



//RPCs



};

#endif