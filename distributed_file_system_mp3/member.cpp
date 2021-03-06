#include "member.h"



std::string coordinator;
std::string machine_ip;
int machine_id=-1;
int highest_id=-1;
std::map<std::string,int> ip_mapping;
std::string failure_process;

bool alive_member::add(std::string ip){
	// static int id_cnt=0;
	if(ip.empty()) return false;
	std::lock_guard<std::mutex> guard(mutex);
	highest_id=std::max(ip_mapping[ip],highest_id);
	if(ip_mapping[ip]==highest_id) coordinator=ip;
	for(auto x:_am){
		if(x.ip==ip) return false;
	}
	if(ip==machine_ip) machine_id=ip_mapping[ip];
	_am.push_back(machine_info(ip,ip_mapping[ip]));

	return true;
}

void alive_member::remove(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	// auto it = std::find(_am.begin(), _am.end(), ip);
	for(auto it=_am.begin();it!=_am.end();++it){
		if((*it).ip==ip){
			if((*it).id==highest_id){
				int second_highest_id=-1;
				for(auto itt=_am.begin();itt!=_am.end();++itt){
					int curid=(*itt).id;
					if(curid>=second_highest_id &&  curid!=highest_id){
						second_highest_id=curid;
					}
				}
				highest_id=second_highest_id;

			}
			
			_am.erase(it);

			break;
		}
	}
}

bool alive_member::exists(std::string ip){
	std::lock_guard<std::mutex> guard(mutex);
	for(auto x:_am){
		if(x.ip==ip) return true;
	}
	return false;
}

std::vector<std::string> alive_member::get_alive_member(){
	std::lock_guard<std::mutex> guard(mutex);
	std::vector<std::string> ret;
	for(auto x:_am){
		ret.push_back(x.ip);
	}
	return ret;
}

std::vector<std::string> alive_member::random_select_K(size_t K,std::vector<std::string> v){
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
		ret+=x.ip;
		ret+=":";
		ret+=std::to_string(x.id);
		ret+=" ";
	}
	return ret;
}

std::vector<machine_info> alive_member::get_alive_member_with_id(){
	std::lock_guard<std::mutex> guard(mutex);
	return _am;
}