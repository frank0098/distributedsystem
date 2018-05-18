#ifndef ELECTION_H
#define ELECTION_H

#include "node.h"
#include "config.h"
#include "logger.h"
#include "state_manager.h"
#include "network_manager.h"
#include "membership.h"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <assert.h>


class Membership;

class Election_manager{

public:
	Election_manager(State_manager* sm);
	~Election_manager();
	void stop();
	void start_election();
	void run_election();
private:
	void election_listner();
	bool _running;
	bool _election_in_process;
	mutable std::mutex _m;
	std::condition_variable _cv;
	
	std::thread _election_thread;
	std::thread _election_client_thread;
	State_manager* _sm;
	Network_UDP* _nw_server;
	Network_UDP* _nw_client;
	Membership* _membership;

	unsigned int _id;
	unsigned int _highest_id;
	char _hostname[IP_LENGTH];
	char _m_election_server_port[PORT_LENGTH];
	char _m_election_client_port[PORT_LENGTH];


};

#endif