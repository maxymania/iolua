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


