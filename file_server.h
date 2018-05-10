#ifndef FILE_SERVER_H
#define FILE_SERVER_H

#include "logger.h"
#include "config.h"

#include "rpc/server.h"
#include "rpc/client.h"


#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <fstream>





using std::cout;
using std::endl;
using std::string;

struct File_data{
string filename;
string date;
string path;
bool available=false;
};
class File_server{
public:
	void start();
	void stop();
private:
	bool rpc_if_exist(string filename);
	vector<string> rpc_list_file();
	string rpc_download_file(string filename);
	bool rpc_upload_file(string filename,string content);
	void rpc_delete_file(string filename);
	void rpc_mark_delete(string filename);
	void save_mem_to_disk();
	std::mutex _fs_lock;
	std::unordered_map<string,File_data> _map;
	std::thread _worker_thread;
	std::unique_ptr<rpc::server> _rpc_server;
	bool _running;




//RPCs



};

#endif