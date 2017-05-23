#ifndef SERVER_H
#define SERVER_H
#include "thread.h"
#include "network.h"

class server{
public:
	void run();
private:
	logger* _logger;
	network* _nw;


};
#endif