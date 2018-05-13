#include "file_manager.h"

File_manager::~File_manager(){
	if(_server_thread.joinable()) _server_thread.join();
	if(_nw) delete _nw;
}

void File_manager::start(){
	fs.start();
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
		while(true){
			_nw->connect();
			
		}
	});
}
void File_manager::stop(){
	fs.stop();
	_running=false;
	_nw->disconnect();
}

unsigned int hash_str(const char* s)
{
#define A 54059 /* a prime */
#define B 76963 /* another prime */
#define C 86969 /* yet another prime */
#define FIRSTH 37 /* also prime */
   unsigned int h = FIRSTH;
   while (*s) {
     h = (h * A) ^ (s[0] * B);
     s++;
   }
   return h; // or return h % C;
}

static bool id_in_range(int first_replica,int size){
	if(size<=REPLICA_CNT) return true;
	int lo=first_replica;
	int hi=(REPLICA_CNT+lo-1)%size;
	int self_id=get_config()->id;
	if(lo<hi){
		if(lo<=self_id && self_id<=hi) return true;
	}
	else if(lo>hi){//wrapped
		if(self_id>=lo||self_id<=hi) return true;
	}
	return false;
}

bool File_manager::rpc_if_exist(string filename){
	unsigned int id=hash_str(filename.c_str())%_size;
	if(id_in_range(id,_size)){
		return fs.rpc_if_exist(filename);
	}

	bool ret=false;
	int cnt=std::min(_size,REPLICA_CNT);
	for(int i=0;i<cnt;i++){
		if(_sm->alive((id+i)%cnt)){
			Node node=_sm->get_peer((id+i)%cnt);
			try{
				ret|=Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"if_exist").as<bool>();
				break;
			}
			catch(...){
				continue;
			}
		}
	}
	return ret;
}

string File_manager::rpc_download_file(string filename){
	unsigned int id=hash_str(filename.c_str())%_size;
	string content;
	if(id_in_range(id,_size)){
		if(fs.rpc_if_exist(filename)){
			return fs.rpc_download_file(filename);
		}
	}
	int cnt=std::min(_size,REPLICA_CNT);
	for(int i=0;i<cnt;i++){
		if(_sm->alive((id+i)%cnt)){
			Node node=_sm->get_peer((id+i)%cnt);
			try{
				content = Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"download_file",filename).as<string>();
				break;
			}
			catch(...){
				continue;
			}
		}
	}
	return content;
}
bool File_manager::rpc_upload_file(string filename,string content){
	int cnt=std::min(_size,REPLICA_CNT);
	for(int i=0;i<cnt;i++){
		if(_sm->alive((_id+i)%cnt)){
			Node node=_sm->get_peer((_id+i)%cnt);
			try{
				Network_RPC::call_rpc(node.peer_ip,node.file_server_port,"upload_file",filename,content);
				break;
			}
			catch(...){
				continue;
			}
		}
	}

	return true;
}

void File_manager::rpc_delete_file(string filename){

}