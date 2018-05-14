#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H


#include "node.h"
#include "state_manager.h"
#include "network_manager.h"
#include "file_server.h"

#include <string>
#include <exception>


using std::string;
using std::vector;
class File_manager{
public:
	File_manager(State_manager* sm);
	~File_manager();
	void start();
	void stop();
private:

	bool rpc_if_exist(string filename);
	string rpc_list_file();
	vector<vector<string>> rpc_list_all_files();
	// pair<string,string> rpc_download_file_addr(string filename);
	string rpc_download_file(string filename);
	bool rpc_upload_file(string filename,string content);
	void rpc_delete_file(string filename);
	void rpc_mark_delete(string filename);

	bool _running;
	File_server* _fs;
	State_manager* _sm;
	int _size;
	int _id;
	Network_RPC* _nw;
	std::thread _server_thread;

};

#endif