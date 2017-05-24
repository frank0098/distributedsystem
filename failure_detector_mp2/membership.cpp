#include "membership.h"


void server_addr_read_config(string config_file_path,std::list<string>& all_member){
	std::ifstream in(config_file_path);
	string ip_addr;
	struct sockaddr_in sa;
	while(in>>ip_addr){
		const char* tmp_addr=ip_addr.c_str(); 
		if(inet_pton(AF_INET,tmp_addr,&(sa.sin_addr))==0 &&
		 inet_pton(AF_INET6,tmp_addr,&(sa.sin_addr))==0){
			throw std::runtime_error("invalid ip_addr: "+ip_addr);
		}
		else{
			all_member.push_back(ip_addr);
		}
	}
}


membership::membership(){
	_logger=new loggerThread("$HOME/log");
	_sv=new server(this->_logger);
	_dt=new detector(&(this->_members),&(this->_alive_members),this->_logger);
	_ds=new dissemination(this->_logger);
	server_addr_read_config("server.cfg",_members);
}

void membership::start(){
	_sv->start();
	_logger->start();
	_dt->start();
}

membership::~membership(){
	_sv->join();
	_logger->join();
	_dt->join();
	delete _sv;
	delete _logger;
	delete _ds;
	delete _dt;
}