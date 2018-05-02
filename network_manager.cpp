#include "network_manager.h"




void Network_Multicast::run_server(){
	printf("runserver\n");
	char databuf[1024] = "Multicast test message lol!";
	int datalen = sizeof(databuf);

	unsigned int port=7000;
	struct sockaddr_in localSock;
	struct ip_mreq group;

	int sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
		printf("Opening datagram socket....OK.\n");
	int reuse = 0;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
	{
		printf("Setting SO_REUSEADDR...OK.\n");
	}

	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(port);
	localSock.sin_addr.s_addr = inet_addr(multicast_group);;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
	perror("Binding datagram socket error");
	close(sd);
	exit(1);
	}
	else
	printf("Binding datagram socket...OK.\n");

	group.imr_multiaddr.s_addr = inet_addr(multicast_group);
	group.imr_interface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
	perror("Adding multicast group error");
	close(sd);
	exit(1);
	}
	else
	printf("Adding multicast group...OK.\n");

	datalen = sizeof(databuf);
	while(true){
		if(read(sd, databuf, datalen) < 0)
		{
			perror("Reading datagram message error");
			close(sd);
			exit(1);
		}
		else
		{

			socklen_t len;
			struct sockaddr_storage addr;
			char ipstr[INET6_ADDRSTRLEN];
			int port;
			getpeername(sd, (struct sockaddr*)&addr, &len);

			printf("The message from multicast server is: \"%s\"\n", databuf);

			if (addr.ss_family == AF_INET) {
			    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
			    port = ntohs(s->sin_port);
			    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
			} else { // AF_INET6
			    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
			    port = ntohs(s->sin6_port);
			    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
			}

			printf("Peer IP address: %s\n", ipstr);

		}		
	}

}

void Network_Multicast::multicast(){

	char databuf[1024] = "Multicast test message lol!";
	int datalen = sizeof(databuf);

	unsigned int port=7000;
	struct in_addr localInterface;
	struct sockaddr_in groupSock;

	int sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr(multicast_group);
	groupSock.sin_port = htons(port);

	localInterface.s_addr = inet_addr("127.0.0.1");

	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");

	if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	{perror("Sending datagram message error");}
	else
	  printf("Sending datagram message...OK\n");

}