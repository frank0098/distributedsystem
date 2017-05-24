#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H


#include "server.h"
#include "dissemination.h"
#include "detector.h"
#include "logger.h"
#include <arpa/inet.h>

class membership
{
public:
	membership(void);
	~membership();
	void start();
private:
	server* _sv;
	detector* _dt;
	dissemination* _ds;
	loggerThread* _logger;
	std::list<string> _members;
	std::list<string> _alive_members;
};
#endif