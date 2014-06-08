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

