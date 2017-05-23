#ifndef SERVER_H
#define SERVER_H
#include "thread.h"

class server::public Thread{
public:
	void run();
private:
	void receiving();
	logger* _log;
	detector* _det;
	dissemination* _dis;
	const int _machine_count;
	vector<string> _members;

};
#endif