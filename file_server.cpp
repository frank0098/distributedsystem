#include "file_server.h"


inline const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void File_server::start(){
	_running=true;
	_rpc_server->bind("if_exist",[&](string filename){
		return this->rpc_if_exist(filename);
	});
	_rpc_server->bind("download_file",[&](string filename){
		return this->rpc_download_file(filename);
	});
	_rpc_server->bind("upload_file",[&](string filename,string content){
		return this->rpc_upload_file(filename,content);
	});
	_rpc_server->bind("delete_file",[&](string filename){
		this->rpc_delete_file(filename);
	});
	_rpc_server->bind("mark_delete",[&](string filename){
		this->rpc_delete_file(filename);
	});
	_rpc_server->run();


}
void File_server::stop(){
	_running=false;
	_rpc_server->stop();
}
bool File_server::rpc_if_exist(string filename){
	std::lock_guard<std::mutex> lg(_fs_lock);
	return _map.find(filename)!=_map.end();
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
	return content;
}
bool File_server::rpc_upload_file(string filename,string content){
	string path=get_config()->fs_path+"/"+filename;
	std::ofstream outfile(path,std::ios_base::out);
	if(!outfile.is_open()){
		return false;
	}
	outfile<<content;
	File_data data;
	data.filename=filename;
	data.date=currentDateTime();
	data.path=path;
	std::lock_guard<std::mutex> lg(_fs_lock);
	_map[filename]=data;
	return true;
}
void File_server::rpc_delete_file(string filename){


}

void File_server::rpc_mark_delete(string filename){
	

}
