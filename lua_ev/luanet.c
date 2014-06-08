#include "luanet.h"
#include "luaio.h"
#include "lualoop_internals.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "../lua/lauxlib.h"
#include "../luax/scheduler.h"
#include "../nwlib/nw.h"

#define METATABLE_SRV "LUAEV_EVNET.SRV"
#define NETLIB_SRV "LUAIO_NETLIB.SRV"

#define printf ()()()

static int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}
typedef struct evnet_srv{
	ev_io io;
	evloop evl;
	scka sck;
	int num;
	int run;
	int client; /* the current client (if any)*/
}evnet_srv;

static void evnet_io_r_cb(EV_P_ ev_io *io, int revents){
	evnet_srv* efd=(evnet_srv*)io;
	if(efd->num<0){ev_io_stop(EV_A_ io); efd->run=0; return;}
	lua_State* D = (efd->num<0)?NULL:dvm_sched_getsuspended(efd->evl->L,efd->num);
	efd->num=-1;
	if(!D){ev_io_stop(EV_A_ io); efd->run=0; return;}
	int fd = nw_accept(efd->sck);
	if(fd<0)
		lua_pushnil(D);
	else
		evio_pushfd(D,fd);
	lua_resume(D,efd->evl->L,1);
	evloop_unfreeze(efd->evl);
}
static int evnet_SRV_accept(lua_State* L){
	evnet_srv* efd = luaL_checkudata(L,1,METATABLE_SRV);
	if(efd->io.fd<0)return 0;
	evloop evl = efd->evl;
	if(efd->num>=0)return 0;
	if(!(efd->run)){
		ev_io_start(EV_B_(evl) (ev_io*)efd);
		efd->run = 1;
	}
	efd->num = dvm_sched_suspend(L);
	return lua_yield(L,0);
}

static int evnet_SRV_close(lua_State* L){
	evnet_srv* efd = luaL_checkudata(L,1,METATABLE_SRV);
	evloop evl = efd->evl;
	if(efd->run)
		ev_io_stop(EV_B_(evl) (ev_io*)efd);
	efd->run=0;
	if(efd->io.fd>=0)
		close(efd->io.fd);
	efd->io.fd=-1;
	return 0;
}
static int evnet_SRV_gc(lua_State* L){
	evnet_srv* efd = luaL_checkudata(L,1,METATABLE_SRV);
	evloop evl = efd->evl;
	if(efd->run)
		ev_io_stop(EV_B_(evl) (ev_io*)efd);
	if(efd->io.fd>=0)
		close(efd->io.fd);
	return 0;
}

static void evnet_SRV_mt(lua_State* L){
	if(luaL_newmetatable(L,METATABLE_SRV)){
		int top = lua_gettop(L);
		lua_newtable(L);
		lua_pushcfunction(L,evnet_SRV_accept);
		lua_setfield(L,top+1,"accept");
		lua_pushcfunction(L,evnet_SRV_close);
		lua_setfield(L,top+1,"close");
		lua_setfield(L,top,"__index");
		lua_pushcfunction(L,evnet_SRV_gc);
		lua_setfield(L,top,"__gc");
		lua_settop(L,top);
	}
}

static int evnet_wrapsrv(lua_State* L){
	scka* sck = luaL_checkudata(L,1,NETLIB_SRV);
	evnet_srv* efd = lua_newuserdata(L,sizeof(evnet_srv));
	int top = lua_gettop(L);
	efd->evl = evloop_get(L);
	efd->sck = *sck;
	efd->num = -1;
	efd->run = 0;
	efd->client = -1;
	setnonblock(sck->sock);
	ev_io_init((ev_io*)efd,evnet_io_r_cb,sck->sock,EV_READ);
	sck->sock = -1;
	evnet_SRV_mt(L);
	lua_setmetatable(L,top);
	return 1;
}

void evnet_install(lua_State* L){
	lua_register(L,"nw_async",evnet_wrapsrv);
}


