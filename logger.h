#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <stdio.h>
#include <vector>
#include <mutex>
#include <sys/stat.h>

using std::vector;
using std::string;
using std::cout;
using std::endl;
class Logger{
public:
	Logger(){};
	Logger(int peer_id);
	void start();
	void stop();
	void write(string str);
	void iniailize(int peer_id);

	


private:
	std::string _path;
	std::mutex _write_lock;
	int _peerid;
	bool _started;
	std::ofstream _log_stream;

};

extern Logger* logger();
extern void initialize_log(int peer_id);
#endif