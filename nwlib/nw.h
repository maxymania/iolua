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
#ifndef lib2_nw_h
#define lib2_nw_h

struct scka_s{
	int sock;
	int addrl;
};
typedef struct scka_s scka;
scka nw_listen_in4(const char* host,int port);
int nw_connect_in4(const char* host,int port);
scka nw_listen_in6(const char* host,int port);
int nw_connect_in6(const char* host,int port);
scka nw_listen_unix(const char* sckf);
int nw_connect_unix(const char* sckf);

int nw_accept(scka s);

#endif

