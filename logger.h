#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <vector>
#include <mutex>
using std::vector;
using std::string;

class Logger{
public:
	Logger(int peer_id);
	void start();
	void stop();
	void write(string str);
private:
	std::string _path;
	std::mutex _write_lock;
	int _peerid;
	bool _started;
	ofstream _log_stream;
};

static Logger* get_logger(int peer_id){
	static Logger instance(peer_id);
	instance.start();
	return &instance;
}

#endif