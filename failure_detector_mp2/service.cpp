#include "service.h"


service::service(loggerThread *lg):_lg(lg){

	_nw=new network_udp(SERVICEPORT,false);
	_nw->connect();
	_lg->add_write_log_task("service start");
}

service::~service(){
	cout<<"service destructor"<<endl;
	_nw->disconnect();
	// _lg->add_write_log_task("service end");
	cout<<"service ends"<<endl;
}

void* service::run(){
	while(true){
		stop_flag.lock();
		if(stop_flag.is_true()){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		char source[INET6_ADDRSTRLEN];
		msg_t msgtype=_nw->recv_msg(source);
		msg_t ret_msgtype=msg_t::UNKNOWN;
		switch(msgtype){
			case msg_t::CONTROLLER_RESTART_REMOTELY:
			pause_flag.lock();
			pause_flag.set_false();
			pause_flag.cond_signal();
			pause_flag.unlock();
			ret_msgtype=msg_t::ACK;
			break;
			case msg_t::CONTROLLER_PAUSE_REMOTELY:
			pause_flag.lock();
			pause_flag.set_true();
			pause_flag.unlock();
			ret_msgtype=msg_t::ACK;
			break;
			case msg_t::CONTROLLER_END_REMOTELY:
			pause_flag.lock();
			pause_flag.set_false();
			pause_flag.cond_signal();
			pause_flag.unlock();
			stop_flag.lock();
			stop_flag.set_true();
			stop_flag.unlock();
			ret_msgtype=msg_t::ACK;
			_lg->end_loggerThread();
			break;
			default:
			break;

		}
		network_udp::send_msg(ret_msgtype,SERVICEPORT,source);
	}
}