#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include "network_manager.h"
#include "logger.h"


#include <string>
#include <vector>


//SWIM Infection-style Dissemination
class Membership{
public:
void run_membership_server();
void run_membership_client();
void start();
void end();

private:
	string _hostname;
	string _m_server_port;
	string _m_client_port;
	vector<Peer_struct> _peers_v;
	bool _running;
	Network_UDP* _nw_server;
	Network_UDP* _nw_client;



//RPCs



};

#endif