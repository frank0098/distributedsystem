#include "file_server.h"


inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
File_server::~File_server(){
	if(_server_thread.joinable()) _server_thread.join();
	if(_nw) delete _nw;
}
void File_server::start(){
	Config* conf=get_config();
	string hostname=conf->peer_ip[conf->id];
	string file_server_port=conf->file_server_port[conf->id];
	_nw=new Network_RPC(hostname.c_str(),file_server_port.c_str());
	_running=true;
	_nw->bind("if_exist",[&](string filename){
		return this->rpc_if_exist(filename);
	});
	_nw->bind("download_file",[&](string filename){
		return this->rpc_download_file(filename);
	});
	_nw->bind("upload_file",[&](string filename,string content){
		return this->rpc_upload_file(filename,content);
	});
	_nw->bind("delete_file",[&](string filename){
		this->rpc_delete_file(filename);
	});
	_nw->bind("mark_delete",[&](string filename){
		this->rpc_delete_file(filename);
	});
	_server_thread=std::thread([&](){
		while(_running){
			_nw->connect();
		}
	});
}

void File_server::stop(){
	_running=false;
	_nw->disconnect();
}
bool File_server::rpc_if_exist(string filename){
	std::lock_guard<std::mutex> lg(_fs_lock);
	auto it=_map.find(filename);
	return it!=_map.end() && it->second.available;
}
vector<string> File_server::rpc_list_file(){
	vector<string> ret;
	std::lock_guard<std::mutex> lg(_fs_lock);
	for(auto &x:_map){
		ret.push_back(x.first);
	}
	return ret;
}
string File_server::rpc_download_file(string filename){
	logger()->write("rpc download enter "+filename);
	string content;
	string path;
	bool exist=false;
	{
		std::lock_guard<std::mutex> lg(_fs_lock);
		auto it=_map.begin();
		for(;it!=_map.end();++it){
			if(it->first==filename){
				path=it->second.path;
				exist=true;
			}
		}
	}
	if(exist){
		std::ifstream f;
		f.open(path);
		if(f.is_open()){
			string line;
			while(std::getline(f,line)){
				content+=line;
			}
		}
	}
	else{
		logger()->write("rpc download UNEXPECTED: FILE DOESNT EXISTrpc download enter "+filename);
	}
	logger()->write("rpc download finished "+filename);
	return content;
}
bool File_server::rpc_upload_file(string filename,string content){
	logger()->write("rpc upload enter "+filename);
	struct flock lock;
	string path=get_config()->fs_path+"/"+filename;
	int fd=open(path.c_str(),O_WRONLY|O_CREAT);
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_WRLCK;
	int lockres=fcntl(fd,F_SETLK,&lock);//lock this file
	if(lockres<0) return false;
	write(fd,content.c_str(),content.size());

	File_data data;
	data.filename=filename;
	data.date=currentDateTime();
	data.path=path;
	std::lock_guard<std::mutex> lg(_fs_lock);
	_map[filename]=data;

	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	_q.push(filename);//enqueue to send replica to to other machines
	logger()->write("rpc upload finished "+filename);
	return true;
}
void File_server::process_replicas(){
	while(_running){
		string filename;
		_q.wait_and_pop(filename);
		string content=rpc_download_file(filename);
		int cnt=std::min(_size,REPLICA_CNT);
		bool write_replica_success=true;
		for(int i=1;i<cnt;i++){
			if(_sm->alive((_id+i)%cnt)){
				Node node=_sm->get_peer((_id+i)&cnt);
				try{
					Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"upload_file",filename,content);
				}
				catch(...){
					logger()->write("FATAL UNABLE TO SEND REPLICA"+node.peer_ip+":"+std::to_string(node.file_server_port));
					_q.push(filename);
					write_replica_success=false;
					break;
				}
			}
		}
		if(write_replica_success){
			for(int i=1;i<cnt;i++){
			if(_sm->alive((_id+i)%cnt)){
				Node node=_sm->get_peer((_id+i)&cnt);
				try{
					Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"mark_available");
				}
				catch(...){
					logger()->write("FATAL UNEXPECTED UNABLE TO MARK AVAILABLE"+node.peer_ip+":"+std::to_string(node.file_server_port));
					}
				}
			}
		}
	}
}
void File_server::rpc_mark_available(string filename){
	std::lock_guard<std::mutex> lg(_fs_lock);
	auto it=_map.find(filename);
	if(it!=_map.end()){
		it->second.available=true;
	}
}

void File_server::rpc_delete_file(string filename){


}

void File_server::rpc_mark_delete(string filename){
	

}
