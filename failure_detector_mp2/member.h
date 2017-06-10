#ifndef MEMBER_H
#define MEMBER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include <iostream>

class alive_member{
public:
	bool add(std::string ip);
	void remove(std::string ip);
	static std::vector<std::string> random_select_K(size_t K,std::string self,std::vector<std::string> v);
	std::vector<std::string> get_alive_member();
	std::string get_alive_member_list();
	bool exists(std::string ip);

private:
	std::vector<std::string> _am;
	std::mutex mutex;
};

#endif