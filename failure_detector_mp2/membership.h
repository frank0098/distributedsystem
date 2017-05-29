#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H


#include "server.h"
#include "dissemination.h"
#include "detector.h"
#include "logger.h"
#include "member.h"
#include "thread.h"
#include "service.h"
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
	dissemination* _ds;
	loggerThread* _logger;
	std::list<std::string> _members;
	alive_member* _am;
};
#endif