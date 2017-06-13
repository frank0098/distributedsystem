#ifndef LOGGER_H
#define LOGGER_H
#include "thread.h"
#include "workQueue.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

class loggerThread:public Thread{
public:
	loggerThread(std::string path);
	~loggerThread();
	void* run();
	void add_write_log_task(std::string params);
	void end_loggerThread();

private:
	void write_log(std::string content);
	string _log_path;
	wqueue<WorkItem*> *_wq;
	bool _up;

};



#endif