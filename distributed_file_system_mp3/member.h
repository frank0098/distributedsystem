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
#include <map>

struct machine_info{
	std::string ip;
	int id;
	machine_info(std::string ip_,int id_):ip(ip_),id(id_){

	}
};
extern std::string coordinator;
extern std::string machine_ip;
extern int machine_id;
extern int highest_id;
extern std::map<std::string,int> ip_mapping;

class alive_member{
public:
	bool add(std::string ip);
	void remove(std::string ip);
	static std::vector<std::string> random_select_K(size_t K,std::string self,std::vector<std::string> v);
	std::vector<std::string> get_alive_member();
	std::vector<machine_info> get_alive_member_with_id();
	std::string get_alive_member_list();
	bool exists(std::string ip);

private:
	std::vector<machine_info> _am;
	std::mutex mutex;
};

#endif