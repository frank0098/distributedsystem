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
	loggerThread::run();
}
void* loggerThread::run(){
	while(!pause_flag.is_true()){
		cout<<"run"<<endl;
		WorkItem* item=(WorkItem*)(_wq->remove());
		loggerThread::write_log(item->m_message);
		delete item;
		// cout<<"run finished"<<endl;
	}
}

void loggerThread::add_write_log_task(string params){
	if(_up){
		string log_content=params;

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