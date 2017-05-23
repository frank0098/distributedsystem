#include "logger.h"

loggerThread::loggerThread(std::string path):_log_path(path),*_wq(new wqueue<WorkItem>),_up(true){

}
~loggerThread::loggerThread(){
	_up=false;
	loggerThread::run();
}
void loggerThread::run(){
	while(true){
		WorkItem* item=(WorkItem*)wq.remove();
		logger::write_log(item.m_message);
		delete item;
	}
}

void loggerThread::add_write_log_task(string params){
	if(_up){
		strint log_content=params;
		WorkItem *item=new WorkItem(log_content,0);
		wq.add(item);	
	}

}
void loggerThread::write_log(std::string content){
	std::ofstream outfile;
  	outfile.open(_log_path, std::ios_base::app);
  	outfile << content;
}