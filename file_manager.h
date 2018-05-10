#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

class File_manager{
public:
private:

	bool rpc_if_exist(string filename);
	vector<string> rpc_list_file();
	vector<vector<string>> rpc_list_all_files();
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

};

#endif