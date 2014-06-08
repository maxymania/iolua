#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "nw.h"

scka nw_listen_unix(const char* sckf){
	struct sockaddr_un serv_addr;
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path,sckf);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)return (scka){-1,0};
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return (scka){-1,0};
	}
	listen(sockfd,5);
	return (scka){sockfd,sizeof(struct sockaddr_un)};
}
int nw_connect_unix(const char* sckf){
	struct sockaddr_un serv_addr;
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path,sckf);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)return -1;
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return -1;
	}
	return sockfd;
}


