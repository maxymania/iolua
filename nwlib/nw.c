#include <sys/types.h>
#include <sys/socket.h>

#include "nw.h"

int nw_accept(scka s){
	char addr[s.addrl];
	int i=s.addrl;
	return accept(s.sock, (struct sockaddr *) addr, &i);
}


