#include "logger.h"

inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

loggerThread::loggerThread(std::string path):_log_path(path),_wq(new wqueue<WorkItem*>),_up(true){
	ofstream newFile;
	newFile.open(_log_path);
	newFile<<"["<<currentDateTime()<<"]"<<"====Logger Start===="<<endl;
	newFile.close();
}
loggerThread::~loggerThread(){
	_up=false;
	ofstream f;
	f.open(_log_path, std::ios_base::app);
	f<<"["<<currentDateTime()<<"]"<<"====Logger Ends===="<<endl;
	f.close();
}
void* loggerThread::run(){
	while(true){
		stop_flag.lock();
		if(stop_flag.is_true() && _wq->size()==0){
			stop_flag.unlock();
			break;
		}
		stop_flag.unlock();
		WorkItem* item=(WorkItem*)(_wq->remove());
		loggerThread::write_log(item->m_message);
		delete item;
	}
}

void loggerThread::add_write_log_task(string log_content){
	if(_up){
		WorkItem *item=new WorkItem(log_content.c_str(),0);
		_wq->add(item);	
	}

}
void loggerThread::write_log(std::string content){
	std::ofstream outfile;
  	outfile.open(_log_path, std::ios_base::app);
  	outfile<<"["<<currentDateTime()<<"] " << content<<endl;
  	// std::cout<<content<<endl;
  	outfile.close();
}

void loggerThread::end_loggerThread(){
	_wq->end();
}