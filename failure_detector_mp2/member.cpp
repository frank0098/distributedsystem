#include "member.h"


bool alive_member::add(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	auto it = std::find(_am.begin(), _am.end(), ip);
	if(it!=_am.end()) return false;
	
	_am.push_back(ip);
	return true;
}

void alive_member::remove(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	auto it = std::find(_am.begin(), _am.end(), ip);
	if(it != _am.end())
	    _am.erase(it);
}

bool alive_member::exists(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	auto it=std::find(_am.begin(),_am.end(),ip);
	return it!=_am.end();
}

std::vector<std::string> alive_member::get_alive_member(){
	std::lock_guard<std::mutex> guard(mutex);
	return _am;
}

// std::vector<std::string> alive_member::ramdom_select_K(size_t K){
// 	std::lock_guard<std::mutex> guard(mutex);
// 	std::vector<std::string> reservoir(K);
// 	if(K>=_am.size()) return _am;
// 	srand(time(NULL));
// 	size_t i=0;
// 	for (size_t i = 0; i < K; i++)
//         reservoir[i] = _am[i];
//     i=0;
// 	for(auto x:_am){
// 		size_t j=rand()%(i+1);
// 		if(j<K){
// 			reservoir[j]=_am[i];
// 		}
// 		i++;
// 	}
// 	return reservoir;

// }
std::vector<std::string> alive_member::random_select_K(size_t K,std::string self,std::vector<std::string> v){
	if(K>=v.size()) return v;
	std::vector<std::string> reservoir(K);
	srand(time(NULL));
	size_t i=0;
	for (size_t i = 0; i < K; i++)
        reservoir[i] = v[i];
    i=0;
	for(auto x:v){
		size_t j=rand()%(i+1);
		if(j<K){
			reservoir[j]=v[i];
		}
		i++;
	}
	return reservoir;

}
std::string alive_member::get_alive_member_list(){
	std::lock_guard<std::mutex> guard(mutex);
	std::string ret;
	for(auto x:_am){
		ret+=x;
		ret+=" ";
	}
	return ret;
}

 