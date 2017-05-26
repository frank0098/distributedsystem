#include "member.h"


void alive_member::add(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	_am.push_back(ip);
}

void alive_member::remove(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	auto it = std::find(_am.begin(), _am.end(), ip);
	if(it != _am.end())
	    _am.erase(it);
}

std::vector<std::string> alive_member::get_alive_member(){
	std::lock_guard<std::mutex> guard(mutex);
	return _am;
}

std::vector<std::string> alive_member::ramdom_select_K(int K){
	std::vector<std::string> reservoir(_am.size());
	if(K>=_am.size()) return _am;
	srand(time(NULL));
	int i=0;
	for (int i = 0; i < K; i++)
        reservoir[i] = _am[i];
	for(auto x:_am){
		int j=rand()%(i+1);
		if(j<K){
			reservoir[j]=_am[i];
		}
		i++;
	}
	return reservoir;

}

 