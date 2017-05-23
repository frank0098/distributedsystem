#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

#include "server.h"
#include "dissemination.h"
#include "detector.h"
#include "logger.h"

class membership
{
public:
	membership(void);
	~membership();
private:
	server* _sv;
	detector* _dt;
	dissemination* _ds;
	logger* _logger;
	const int _machine_count;
	vector<string> _members;
	vector<string> _alive_members;
};
#endif