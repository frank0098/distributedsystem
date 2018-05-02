#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H
#include "node.h"
#include <string>
#include <vector>
#include <mutex>

using std::string;
class State_manager{
public:
	int leader_id;
	vector<node> peers;
};

#endif