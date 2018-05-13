#ifndef FILE_SERVER_H
#define FILE_SERVER_H

#define REPLICA_CNT 3


#include "logger.h"
#include "config.h"
#include "network_manager.h"
#include "state_manager.h"


#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <fstream>
#include <fcntl.h>


using std::cout;
using std::endl;
using std::string;

struct File_data{
string filename;
string date;
string path;
bool available=false;
};
// class File_manager;

class File_server{
friend class File_manager;
public:
	File_server():_running(false),_nw(nullptr){}
	void start();
	void stop();
	~File_server();
private:
	bool rpc_if_exist(string filename);
	vector<string> rpc_list_file();
	string rpc_download_file(string filename);
	bool rpc_upload_file(string filename,string content);
	void rpc_delete_file(string filename);
	void rpc_mark_delete(string filename);
	void rpc_mark_available(string filename);
	void save_mem_to_disk();
	void process_replicas();
	std::mutex _fs_lock;
	std::unordered_map<string,File_data> _map;
	std::thread _server_thread;
	bool _running;
	Network_RPC* _nw;
	Queue<string> _q;
	int _id;
	int _size;
	State_manager* _sm;




//RPCs



};

#endif