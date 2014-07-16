/*
 * Copyright (c) 2014, Simon Schmidt
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "nw.h"

scka nw_listen_in6(const char* host,int port){
	struct sockaddr_in6 serv_addr;
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_port = htons(port);
	if(host && *host){
		if(!inet_pton(AF_INET6,host,&serv_addr.sin6_addr.s6_addr))
			return (scka){-1,0};
	}else{
		serv_addr.sin6_addr = in6addr_any;
	}
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd<0)return (scka){-1,0};
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return (scka){-1,0};
	}
	listen(sockfd,5);
	return (scka){sockfd,sizeof(struct sockaddr_in6)};
}
int nw_connect_in6(const char* host,int port){
	struct sockaddr_in6 serv_addr;
	serv_addr.sin6_family = AF_INET6;
	serv_addr.sin6_port = htons(10000);
	if(host && *host){
		if(!inet_pton(AF_INET6,host,&serv_addr.sin6_addr.s6_addr))
			return -1;
	}else{
		serv_addr.sin6_addr = in6addr_any;
	}
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd<0)return -1;
	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		close(sockfd);
		return -1;
	}
	return sockfd;
}


