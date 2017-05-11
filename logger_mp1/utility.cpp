#include "utility.h"
string grep_server(const char* cmd){
	array<char,128> buffer;
	string result;
	shared_ptr<FILE> pipe(popen(cmd,"r"),pclose);
	if(!pipe) throw std::runtime_error("popen() failed!");
	while(!feof(pipe.get())){
		if(fgets(buffer.data(),128,pipe.get())!=NULL){
			result+=buffer.data();
		}
	}
	return result;
}