#include "network_manager.h"
#include "node.h"
#include "config.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <array>

#include <typeinfo>


using namespace std;
void list_file(int id){
	Config* conf=get_config();
	string ipaddr=conf->peer_ip[id];
	string file_manager_port=conf->file_manager_port[id];
	try{
		auto res=Network_RPC::call_rpc(ipaddr,std::stoi(file_manager_port),"list_file").as<vector<string>>();
		// cout<<res.type<<endl;
		cout<<"files @ node "<<id<<endl;
		// cout<<res.as<string>()<<endl;
		if(res.size()==0){
			cout<<"no file to display"<<endl;
		}
		for(auto &x:res){
			cout<<x<<endl;
		}
	}
	catch(runtime_error& ex){

		cout<<"error!"<<ex.what()<<endl;
	}
	catch(exception& ex){
		cout<<"error!"<<ex.what()<<endl;
	}
	catch(...){

		cout<<"unknwon exception"<<endl;
	}
}
void upload_file(int id,string path,string filename){
	Config* conf=get_config();
	string ipaddr=conf->peer_ip[id];
	string file_manager_port=conf->file_manager_port[id];
	std::ifstream f;
	f.open(path);
	string data;
	if(f.is_open()){
		string line;
		while(std::getline(f,line)){
			data+=line;
		}
	}
	try{
		bool res=Network_RPC::call_rpc(ipaddr,std::stoi(file_manager_port),"upload_file",filename,data).as<bool>();
		if(res){
			cout<<"upload successfully"<<endl;
		}
		else{
			cout<<"failed to upload pls try again"<<endl;
		}
	}
	catch(runtime_error& ex){
		cout<<"error!"<<ex.what()<<endl;
	}
	catch(exception& ex){
		cout<<"error!"<<ex.what()<<endl;
	}
	catch(...){

		cout<<"unknwon exception"<<endl;
	}

}
void get_file(int id,string filename){
	Config* conf=get_config();
	string ipaddr=conf->peer_ip[id];
	string file_manager_port=conf->file_manager_port[id];
	std::ofstream of;
	try{
		bool ifexist=Network_RPC::call_rpc(ipaddr,std::stoi(file_manager_port),"if_exist",filename).as<bool>();
		if(!ifexist){
			cout<<"file does not exist"<<endl;
			return;
		}
		string content=Network_RPC::call_rpc(ipaddr,std::stoi(file_manager_port),"download_file",filename).as<string>();
		
		of.open("./"+filename);
		of<<content;
		of.close();
		cout<<"saved file "+filename<<endl;
	}
	catch(runtime_error& ex){
		cout<<"error!"<<ex.what()<<endl;
	}
	catch(exception& ex){
		cout<<"error!"<<ex.what()<<endl;
	}
	catch(...){

		cout<<"unknwon exception"<<endl;
	}

}


int main(int argc,char ** argv){
	int id=0;
	if (argc<2){
		fprintf(stderr,"usage: ./client.out ops [filename] \n");
	    exit(1);
	}
	if(strcmp(argv[1],"GET")==0||strcmp(argv[1],"get")==0){
		if(argc<3){
			fprintf(stderr,"usage: ./client.out GET [filename] \n");
	    	exit(1);
		}
		get_file(id,argv[2]);
	}
	else if(strcmp(argv[1],"POST")==0||strcmp(argv[1],"post")==0){
		if(argc<4){
			fprintf(stderr,"usage: ./client.out POST [path] [filename] \n");
	    	exit(1);
		}
		upload_file(id,argv[2],argv[3]);
	}
	else if(strcmp(argv[1],"LS")==0||strcmp(argv[1],"ls")==0){
		list_file(id);
	}
	return 0;
}