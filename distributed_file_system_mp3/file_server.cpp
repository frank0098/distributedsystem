#include "file_server.h"

file_server::file_server(loggerThread* lg,alive_member* am):_lg(lg),_am(am){
	_lg->add_write_log_task("server start");
	_nw=new network_server(FILE_SERVER_PORT,lg);
	_nw->connect();
}
file_server::~file_server(){
	_nw->disconnect();
	delete _nw;
	_lg->add_write_log_task("file server ends");
}

void* file_server::run(){
	_nw->serve_forever();
}