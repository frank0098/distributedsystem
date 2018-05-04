#include "logger.h"


static Logger instance;

Logger* logger(){
	return &instance;
}
void initialize_log(int peer_id){
	instance.iniailize(peer_id);
}

inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
void Logger::iniailize(int id){

	_started=false;
	mkdir("log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	_path="log/peer"+std::to_string(id)+".log";
	_peerid=id;
	start();
}
void Logger::start(){
	if(!_started){
		_log_stream.open(_path,std::ios_base::out);
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
	if(_started){
		cout<<"write"<<content<<endl;
		std::lock_guard<std::mutex> lg(_write_lock);
		_log_stream<<"["<<currentDateTime()<<"] " << content<<endl;
	}
}