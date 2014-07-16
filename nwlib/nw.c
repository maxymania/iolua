/*
 * Copyright (c) 2014, Simon Schmidt
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#include <sys/types.h>
#include <sys/socket.h>

#include "nw.h"

int nw_accept(scka s){
	char addr[s.addrl];
	int i=s.addrl;
	return accept(s.sock, (struct sockaddr *) addr, &i);
}


