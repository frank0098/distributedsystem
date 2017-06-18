#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H


#include "server.h"
#include "detector.h"
#include "logger.h"
#include "member.h"
#include "thread.h"
#include "service.h"
#include "election.h"
#include "file_server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>





class membership
{
public:
	membership(void);
	~membership();
	void start();
private:
	server* _sv;
	service* _sc;
	detector* _dt;
	detector_sender* _dts;
	election* _el;
	election_listener* _ell;
	loggerThread* _logger;
	std::list<std::string> _members;
	alive_member* _am;
	file_server* _fs;
};
#endif