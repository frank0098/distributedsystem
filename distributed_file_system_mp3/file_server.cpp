#include "file_server.h"

file_server::file_server(loggerThread* lg,alive_member* am):_lg(lg),_am(am){
	_lg->add_write_log_task("server start");
	_nw=new network_server(FILE_SERVER_PORT,lg);
	_nw->connect();
}
file_server::~file_server(){
	_nw->disconnect();
	delete _nw;
	_lg->add_write_log_task("file server ends");
}

void* file_server::run(){
	_nw->serve_forever(_am,&file_addr_map);
}


// const char *server_response_msg = "HTTP/1.1 %s\r\nThe file requested is %s with SIZE %s\r\n\r\n";
// const char *client_msg="%s %s HTTP/1.1\r\nUser-Agent: %s\r\nHost: %s\r\nConnection: %s\r\nSIZE: %s \r\n\r\n";

void network_server::serve_forever(alive_member* am,std::unordered_map<std::string,std::vector<string> > *file_addr_map) {
    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    int new_fd;
    char s[INET6_ADDRSTRLEN];
    int numbytes=0;
    char buf[MAXDATASIZE];
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    while(1) {
        sin_size=sizeof their_addr;
        new_fd=accept(_sockfd,(struct sockaddr*)&their_addr,&sin_size);
        if(new_fd==-1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr*)&their_addr),
                  s,sizeof s);
        if((numbytes=recv(new_fd,buf,MAXDATASIZE,0)) == -1) {
            perror("recv");
            continue;
        }
        char filename[30];
        char dummy[30];
        char request_type[30];
        char requested_file_size[30];
        char info[100];
        sscanf(buf,client_msg,request_type,filename,dummy,dummy,dummy,requested_file_size,info);
        if(_lg) {
            _lg->add_write_log_task("Server recv "+string(request_type)+" type filename: "+filename +"from "+string(s));
        }

        char file[200];
        strcpy(file,homedir);
        strcat(file,"/dfs/");
        strcat(file,filename);
        // cout<<"filename: "<<file<<endl;

        if (!fork())
        {   // this is the child process
            close(_sockfd); // child doesn't need the listener
            int file_size=0;

            char info[30];
            char buf[BUFFER_SIZE];
            char response[BUFFER_SIZE];
            if(strcmp(request_type,"GET")==0) {
                std::ifstream fin(file,std::ios_base::in);
                if(fin.is_open()) {
                    char sizemsg[200];
                    strcpy(info,"200");
                    struct stat st;
                    stat(file,&st);
                    file_size=st.st_size;
                    sprintf(sizemsg, "%d", file_size);
                    int file_size_left=file_size;
                    sprintf(response, server_response_msg, info, filename,sizemsg);

                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                    while(fin) {
                        fin.read(buf,BUFFER_SIZE);
                        int cur_send=send(new_fd,buf,std::min(BUFFER_SIZE,file_size_left),0);
                        if(cur_send<0) {
                            perror("cannot send");
                            exit(1);
                        }
                        file_size_left-=cur_send;
                    }

                }
                else {
                    perror("cannot open the file...\n");
                    strcpy(info,"404");
                    sprintf(response, server_response_msg, info,filename,"0");
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                }
            }
            else if(strcmp(request_type,"POST")==0) {
            	cout<<"post branch"<<file<<endl;
                std::ofstream outfile(file);
                outfile.close();
                outfile.open(file, std::ios_base::app);
                if(outfile.is_open()) {
                    strcpy(info,"200");
                    sprintf(response, server_response_msg, info, filename,requested_file_size);
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                    cout<<"requested_file_size: "<<requested_file_size<<endl;
                    int file_size_left=atoi(requested_file_size);
                    while(file_size_left>0) {
                        buf[0]='\0';
                        int cur_recv=recv(new_fd,buf,std::min(BUFFER_SIZE,file_size_left),0);
                        if(cur_recv<=0) {
                            perror("file_server_recv_post");
                            exit(1);
                        }
                        file_size_left-=cur_recv;
                        buf[cur_recv]='\0';
                        outfile<<buf;
                    }

                }
                else {
                    perror("cannot open the file...\n");
                    strcpy(info,"404");
                    char response[BUFFER_SIZE];
                    sprintf(response, server_response_msg, info,filename,"0");
                    if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                    }
                }
                outfile.close();

            }
            else if(strcmp(request_type,"DELETE")==0) {
            	if(remove(file)==0){
            		_lg->add_write_log_task("File "+string(file)+" has been removed");
            		strcpy(info,"200");
            	}
            	else{
            		_lg->add_write_log_task("File "+string(file)+" doesnt exist");
            		strcpy(info,"404");
            	}
                sprintf(response, server_response_msg, info,filename,"0");
            	if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            }
            else if(strcmp(request_type,"LS")==0){
            	DIR *dir;
            	struct dirent* ent;
            	char path_name[100];
            	strcpy(path_name,homedir);
            	strcat(path_name,"/dfs/");
            	string folder_files="";
            	if((dir=opendir(path_name))!=nullptr){
            		while((ent=readdir(dir))!=nullptr){
            			string fname=string(ent->d_name);
            			if(fname!="." and fname !=".."){
            				folder_files+=string(ent->d_name);
            				folder_files+="\t";
            			}
            			
            		}
            		closedir(dir);
            		strcpy(info,"200");
            	}
            	else{
            		_lg->add_write_log_task("Fail to open folder ~/dfs");
            		strcpy(info,"200");
            	}
            	cout<<"folder file "<<folder_files<<endl;
            	sprintf(response, server_response_msg, info,filename,to_string(folder_files.size()).c_str());
            	if(send(new_fd,response,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            	if(send(new_fd,folder_files.c_str(),folder_files.size(),0)<0){
            		perror("cannot send");
            		exit(1);
            	}

            }


            else if(strcmp(request_type,"COORDINATOR")==0){
            	char cord[INET6_ADDRSTRLEN];
            	cord[0]='\0';
            	strcpy(cord,coordinator.c_str());
            	if(send(new_fd,cord,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            }

            // else if(strcmp(request_type,"LIST_ALL_SERVER")==0){
            // 	string tmp="";
            // 	for(auto x:am->get_alive_member()){
            // 		tmp+=x;
            // 		tmp+="\t";
            // 	}
            // 	char resp[BUFFER_SIZE];
            // 	resp[0]='\0';
            // 	strcpy(resp,tmp.c_str());
            // 	if(send(new_fd,resp,BUFFER_SIZE,0)<0){
            // 		perror("cannot send");
            // 		exit(1);
            // 	}

            // }

            else if(strcmp(request_type,"GET_FILE_ADDR_ONE")==0){
            	int rnd = rand()%3; 
	    		string fn=string(filename);
	    		char file_addr[INET6_ADDRSTRLEN];
	    		file_addr[0]='\0';
	    		if(file_addr_map->find(fn)==file_addr_map->end()){
	    			strcpy(file_addr,"404");
	    		}
	    		else{
	    			strcpy(file_addr,file_addr_map->at(fn)[rnd].c_str());
	    		}
	    		if(send(new_fd,file_addr,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }

            }
            else if(strcmp(request_type,"GET_FILE_ADDR_ALL")==0){
            	string fn=string(filename);
            	char resp[BUFFER_SIZE];
            	resp[0]='\0';
				if(file_addr_map->find(fn)==file_addr_map->end()){
					strcpy(resp,"404");
				}
				else{
					string tmp_str="";
					std::vector<string> v=file_addr_map->at(fn);
					for(auto x:v){
						tmp_str+=x;
						tmp_str+="\t";
					}
					strcpy(resp,tmp_str.c_str());
				}
				if(send(new_fd,resp,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }

            }
			else if(strcmp(request_type,"LIST_ALL_FILES")==0){
            	char resp[BUFFER_SIZE];
            	resp[0]='\0';
            	string tmp_str;
            	for(auto it=file_addr_map->begin();it!=file_addr_map->end();++it){
            		tmp_str+=it->first;
            		tmp_str+="\t";
            	}
            	strcpy(resp,tmp_str.c_str());
            	if(send(new_fd,resp,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }
            }

            else if(strcmp(request_type,"REQUEST_POST_FILE")==0){
            	string fn=string(filename);
            	char resp[BUFFER_SIZE];
            	resp[0]='\0';
				if(file_addr_map->find(fn)==file_addr_map->end()){
					if(am->get_alive_member().size()<DUPLICATE_COUNT){
						strcpy(resp,"500");//violate consistency requirement
					}
					else{
						std::vector<string> v=alive_member::random_select_K(3,am->get_alive_member());
						string tmp_str="";
						for(auto x:v){
							tmp_str+=x;
							tmp_str+="\t";
						}
						strcpy(resp,tmp_str.c_str());
					}

				}
				else{
					string tmp_str="";
					for(auto x:file_addr_map->at(fn)){
						tmp_str+=x;
						tmp_str+="\t";
					}
					strcpy(resp,tmp_str.c_str());
				}
				if(send(new_fd,resp,BUFFER_SIZE,0)<0) {
                        perror("cannot send");
                        exit(1);
                }

            }
            else if(strcmp(request_type,"INSERT_FILE_ENTRY")==0){
            	std::vector<string> v;
            	
            	string ip_addrs=string(info);
            	size_t prev_loc=0;
            	for(size_t i=0;i<ip_addrs.size();++i){
            		if(ip_addrs[i]=='\t'){
            			v.push_back(ip_addrs.substr(prev_loc,i-prev_loc));
            			prev_loc=i+1;
            		}
            	}
            	file_addr_map->at(string(filename))=v;

            }
            else if(strcmp(request_type,"DELETE_FILE_ENTRY")==0){
            	file_addr_map->erase(string(filename));
            }



            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }

}