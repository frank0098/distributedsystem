#include "membership.h"

void membership::membership(){
	_logger=new logger("$HOME/membershiplog");
	_sv=new server();
	_dt=new detector();
	_ds=new dissemination();
}
void membership::start(){
	_sv->start();
	_logger->start();
	_dt->start();
}

void membership::~membership(){
	_sv->join();
	_logger->join();
	_dt->join();
	delete _sv;
	delete _logger;
	delete _ds;
	delete _dt;
}