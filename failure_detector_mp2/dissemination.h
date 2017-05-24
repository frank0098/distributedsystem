#ifndef DISSEMINATION_H
#define DISSEMINATION_H
#include "network.h"
#include "logger.h"

class dissemination{
public:
	dissemination(loggerThread *lg);
private:
	loggerThread* _lg;
	network* _nw;

};
#endif