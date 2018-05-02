#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using std::string;
// class Network_RPC:public Network{
// 	void connect();
// 	void run_server(string dest_ip,unsigned short port);
// };

class Network{
public:
	// Network();
	virtual void run_server()=0;
	virtual void multicast()=0;
	// virtual send(string dest_ip,unsigned short port);
private:
	bool _running;
};

class Network_Multicast:public Network{
public:
	void run_server();
	void multicast();
private:
	const char* multicast_group="226.1.1.1";
};
#endif



// here is one (trimmed) example of using select()

// INT32    selectStatus;                                 /* select() return code */

// char     tempreport[ 256 ] = {'\0'};

// struct   timeval tv;

// fd_set   fdread;
// //fd_set   fdwrite;
// //fd_set   fdexcep;



// // note:
// //  must try to read report until no report available 
// // so have latest report in buffer
// do
// {
//     /* Note: timeout must be (re)set every time before call to select() */
//     tv.tv_sec = 1;
//     tv.tv_usec = 0;


//     FD_ZERO(&fdread);
//     FD_SET( FD, &fdread );

//     selectStatus = select(FD+1, &fdread, NULL, NULL, &tv);

//     switch( selectStatus )
//     {
//         case -1:
//             ....
//             break;

//         case 0:
//             // timeout, I.E. nothing to read
//             ....
//             break;

//         default: /* available to read */

//             memset(tempreport, 0x00, sizeof(tempreport) );
//             readStatus = read_UDP_socket( FD, tempreport, sizeof(tempreport), &readCount );

//             break;
//     } // end switch( selectStatus )
// } while( (0 < selectStatus)&&(eRS_Success == readStatus ) );