#ifndef MEMBER_H
#define MEMBER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include <string>
#include <vector>
#include <time.h>

class alive_member{
public:
	void add(std::string ip);
	void remove(std::string ip);
	std::vector<std::string> ramdom_select_K(int K);
	std::vector<std::string> get_alive_member();
	std::string get_alive_member_list();

private:
	std::vector<std::string> _am;
	std::mutex mutex;
};

#endif