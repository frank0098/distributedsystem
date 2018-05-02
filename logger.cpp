#include "logger.h"


inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}
void Logger::Logger(int id){
	_started=false;
	_path="./"+"log/peer"+std::to_string(id)+".log";
	_peerid=id;
}
void Logger::start(){
	if(!_started){
		_log_stream.open(_log_path,std::ios_base::app);
		_log_stream<<"["<<currentDateTime()<<"]"<<"====Logger Start Peer id:"<<_peerid<<"===="<<endl;
		_started=true;
	}
}
void Logger::stop(){
	if(_started){
		_log_stream<<"["<<currentDateTime()<<"]"<<"====Logger Ends===="<<endl;
		_log_stream.close();
		_started=false;
	}
}
void Logger::write(std::string content){
	std::lock_guard lg(_write_lock);
	_log_stream<<"["<<currentDateTime()<<"] " << content<<endl;
}