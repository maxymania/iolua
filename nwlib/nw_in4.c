#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "nw.h"

scka nw_listen_in4(const char* host,int port){
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if(host && *host){
		if(!inet_pton(AF_INET,host,&serv_addr.sin_addr.s_addr))
			return (scka){-1,0};
	}else{
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)return (scka){-1,0};
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return (scka){-1,0};
	}
	listen(sockfd,5);
	return (scka){sockfd,sizeof(struct sockaddr_in)};
}
int nw_connect_in4(const char* host,int port){
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(10000);
	if(host && *host){
		if(!inet_pton(AF_INET,host,&serv_addr.sin_addr.s_addr))
			return -1;
	}else{
		serv_addr.sin_addr.s_addr = INADDR_ANY;
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)return -1;
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return -1;
	}
	return sockfd;
}


