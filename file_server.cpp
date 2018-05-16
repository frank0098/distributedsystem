#include "file_server.h"

const string mem_file="/#mem";

inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
File_server::File_server(State_manager* sm):_running(false),_nw(nullptr),_sm(sm){
	load_dist_to_mem();

	
}
File_server::~File_server(){
	if(_server_thread.joinable()) _server_thread.join();
	if(_process_replica_thread.joinable()) _process_replica_thread.join();
	if(_nw) delete _nw;
}
void File_server::start(){

	_running=true;
	if(!_sm){
		logger()->write("file_server: FATAL State manager is nullptr");
		return;
	}

	Config* conf=get_config();
	_size=conf->peer_ip.size();
	_id=conf->id;
	string hostname=conf->peer_ip[conf->id];
	string file_server_port=conf->file_server_port[conf->id];
	std::cout<<file_server_port<<std::endl;
	_nw=new Network_RPC(hostname.c_str(),file_server_port.c_str());

	
	_process_replica_thread=std::thread([&](){
		process_replicas();
	});
	_nw->bind("if_exist",[&](string filename){
		return this->rpc_if_exist(filename);
	});
	_nw->bind("download_file",[&](string filename){
		return this->rpc_download_file(filename);
	});
	_nw->bind("upload_file",[&](string filename,string content,bool iffromclient){
		return this->rpc_upload_file(filename,content,iffromclient);
	});
	_nw->bind("rpc_list_file",[&](){
		return this->rpc_list_file();
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
	logger()->write("file_server: start successfully");
}

void File_server::stop(){
	_q.stop();
	_running=false;
	_nw->disconnect();
}
bool File_server::rpc_if_exist(string filename){
	std::lock_guard<std::mutex> lg(_fs_lock);
	auto it=_map.find(filename);
	if(it!=_map.end()){
		if(it->second.available){
			return true;
		}
		else{
			logger()->write("rpc file is not available"+filename);
		}
	}
	else{
		logger()->write("rpc file doesnt exist"+filename);
	}
	return it!=_map.end() && it->second.available;
}
vector<string> File_server::rpc_list_file(){
	logger()->write("rpc list file");
	vector<string> ret;
	std::lock_guard<std::mutex> lg(_fs_lock);
	for(auto &x:_map){
		ret.push_back(x.first);
	}
	return ret;
}
string File_server::rpc_download_file(string filename){
	logger()->write("rpc download enter filename "+filename);
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
bool File_server::rpc_upload_file(string filename,string content,bool iffromclient){
	logger()->write("rpc upload enter "+filename);
	struct flock lock;
	if(mkdir(get_config()->fs_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1){
		if(errno != EEXIST ){
			logger()->write("failed create directory: "+get_config()->fs_path);
			return false;
		}
	}
	string path=get_config()->fs_path+"/"+filename;
	int fd=open(path.c_str(),O_WRONLY|O_CREAT,0666);
	if(fd<0){
		logger()->write("open file failed: "+path);
		return false;
	}
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_WRLCK;
	int lockres=fcntl(fd,F_SETLK,&lock);//lock this file
	cout<<content<<endl;
	if(lockres<0){
		logger()->write("failed to lock "+path);
		 return false;
	}
	write(fd,content.c_str(),content.size());

	File_data data;
	data.filename=filename;
	data.date=currentDateTime();
	data.path=path;
	data.available=false;
	std::lock_guard<std::mutex> lg(_fs_lock);
	_map[filename]=data;
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
	close(fd);
	if(iffromclient){
		_q.push(filename);//enqueue to send replica to to other machines
	}
	else{
		_map[filename].available=true;
	}
	save_map_to_disk();
	logger()->write("rpc upload finished "+filename);
	return true;
}
void File_server::process_replicas(){
	logger()->write("process replica thread starts");
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
					Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"upload_file",filename,content,false);
				}
				catch(...){
					logger()->write("FATAL UNABLE TO SEND REPLICA"+node.peer_ip+":"+std::to_string(node.file_server_port));
					_q.push(filename);
					write_replica_success=false;
					break;
				}
			}
			else{
				logger()->write(std::to_string((_id+i)%cnt)+"is dead");
			}
		}
		if(write_replica_success){
			logger()->write("successfully write replica");
			_map[filename].available=true;
			save_map_to_disk();
		}
		else{
			logger()->write("write replica fails");
		}
	}
}

void File_server::save_map_to_disk(){
	map<string,Json> mymap;
	for(auto &x:_map){
		map<string,string> tmpobj;
		tmpobj["filename"]=x.second.filename;
		tmpobj["date"]=x.second.date;
		tmpobj["path"]=x.second.path;
		if(x.second.available){
			tmpobj["available"]="true";
		}
		else{
			tmpobj["available"]="false";
		}
		Json obj(tmpobj);
		mymap[x.first]=obj;
	}
	Json json(mymap);
	string data=Json_parser::encode(json);
	// cout<<"memdata"<<data<<endl;
	struct flock lock;
	string path=get_config()->fs_path+mem_file;
	int fd=open(path.c_str(),O_WRONLY|O_CREAT,0666);
	if(fd<1){
		logger()->write("FATAL: cannot create mem file");
	}
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_WRLCK;
	int lockres=fcntl(fd,F_SETLKW,&lock);//lock this file
	write(fd,data.c_str(),data.size());
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,&lock);
	close(fd);
}
void File_server::load_dist_to_mem(){
	string path=get_config()->fs_path+mem_file;
	string data;
	std::ifstream f;
	f.open(path);
	if(f.is_open()){
		string line;
		while(std::getline(f,line)){
			data+=line;
		}
		cout<<data<<endl;
		Json obj=Json_parser::decode(data);
		if(obj.is_object()){
			for(auto &x:obj.object_items()){
				File_data fd;
				fd.filename=x.second["filename"].string_value();
				fd.date=x.second["date"].string_value();
				fd.path=x.second["path"].string_value();
				if(x.second["available"].string_value()=="true"){
					fd.available=true;
				}
				else{
					fd.available=false;
				}
				_map[x.first]=fd;
			}
		}
		f.close();
	}
	cout<<"finished"<<endl;

}

void File_server::rpc_delete_file(string filename){


}

void File_server::rpc_mark_delete(string filename){
	

}
