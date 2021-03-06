#include "membership.h"




void server_addr_read_config(string config_file_path,std::list<string>& all_member){
	std::ifstream in(config_file_path);
	string ip_addr;
	struct sockaddr_in sa;
	int i=0;
	while(in>>ip_addr){
		const char* tmp_addr=ip_addr.c_str(); 
		if(inet_pton(AF_INET,tmp_addr,&(sa.sin_addr))==0 &&
		 inet_pton(AF_INET6,tmp_addr,&(sa.sin_addr))==0){
			throw std::runtime_error("invalid ip_addr: "+ip_addr);
		}
		else{
			all_member.push_back(ip_addr);
			ip_mapping[ip_addr]=i++;
		}
	}
  	machine_ip=string(getenv ("SELFIP"));
  	
}


membership::membership(){
	char buf[200];
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	strcpy(buf,homedir);
	strcat(buf,"/log");
	server_addr_read_config("server.cfg",_members);
	_logger=new loggerThread(buf);
	
	_am=new alive_member();
	_sv=new server(this->_logger,_am);
	_el=new election(this->_logger,_am);
	_ell=new election_listener(this->_logger,_am);
	_dt=new detector(&(this->_members),this->_am,this->_logger);
	_dts=new detector_sender(&(this->_members),this->_am,this->_logger);
	_sc=new service(this->_logger,_sv->get_nw());
	_fs=new file_server(this->_logger,_am);


}

void membership::start(){
	_sc->start();
	_sv->start();
	_logger->start();
	_el->start();
	_ell->start();
	_dt->start();
	_dts->start();
	_fs->start();
	while(true){
		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		
	}
}

membership::~membership(){
	cout<<"end membership"<<endl;
	_fs->join();
	_sv->join();
	_logger->join();
	_dt->join();
	_el->join();
	_ell->join();
	_dts->join();
	_sc->join();
	cout<<"join finished"<<endl;
	delete _fs;
	delete _sc;
	delete _sv;
	delete _logger;
	delete _am;
	delete _dt;
	delete _dts;
	delete _el;
	delete _ell;
}