#include "luaio.h"
#include "lualoop_internals.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "../lua/lauxlib.h"
#include "../luax/scheduler.h"

// for liolib_destroyfd
#include "../lua_io/luaiolib.h"

#define METATABLE_FD "LUAEV_EVIO.FD"

#define MAXSIZE 1024

static int setnonblock(int fd)
{
  int flags;

  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

typedef struct evio_rw_s{
	evloop evl;
	ev_io io_r;
	ev_io io_w;
	int num_r;
	int num_w;
	int run_r;
	int run_w;
	int read_n;
} evio_rw;

static void evio_read_fd(int fd,int n,lua_State* D){
	char buffer[n];
	int m = read(fd,buffer,n);
	if(m>0)
		lua_pushlstring(D,buffer,m);
	else
		lua_pushnil(D);
}
static void evio_io_r_cb(EV_P_ ev_io *io, int revents){
	evio_rw* efd=io->data;
	lua_State* D = (efd->num_r<0)?NULL:dvm_sched_getsuspended(efd->evl->L,efd->num_r);
	efd->num_r=-1;
	if(!D){ev_io_stop(EV_A_ io); efd->run_r=0; return;}
	evio_read_fd(io->fd,efd->read_n,D);
	lua_resume(D,efd->evl->L,1);
	evloop_unfreeze(efd->evl);
}

static int evio_FD_read(lua_State* L){
	lua_settop(L,2);
	evio_rw* efd = luaL_checkudata(L,1,METATABLE_FD);
	evloop evl = efd->evl;
	int s = lua_tointeger(L,2);
	if(s<=0)return 0;
	if(s>MAXSIZE) return 0;
	if(efd->num_r>=0)return 0;
	if(!(efd->run_r)){
		ev_io_start(EV_B_(evl) &(efd->io_r));
		efd->run_r = 1;
	}
	efd->num_r = dvm_sched_suspend(L);
	efd->read_n = s;
	return lua_yield(L,0);
}

static void evio_io_w_cb(EV_P_ ev_io *io, int revents){
	evio_rw* efd=io->data;
	lua_State* D = (efd->num_w<0)?NULL:dvm_sched_getsuspended(efd->evl->L,efd->num_w);
	efd->num_w=-1;
	if(!D){ev_io_stop(EV_A_ io); efd->run_w=0; return;}
	int top = lua_gettop(D);
	int n = lua_tointeger(D,top);
	void* data = lua_touserdata(D,top-1);
	n = write(io->fd,data,n);
	lua_settop(D,0);
	lua_pushinteger(D,n);
	lua_resume(D,efd->evl->L,1);
	evloop_unfreeze(efd->evl);
}
static int evio_FD_write(lua_State* L){
	const char* data;
	size_t size;
	lua_settop(L,4);
	evio_rw* efd = luaL_checkudata(L,1,METATABLE_FD);
	evloop evl = efd->evl;
	int b=0,e=0;
	luaL_checktype(L,2,LUA_TSTRING);
	data = lua_tolstring(L,2,&size);
	e = size;
	if(lua_type(L,3)==LUA_TNUMBER)
		b = lua_tointeger(L,3);
	if(lua_type(L,4)==LUA_TNUMBER)
		e = lua_tointeger(L,4)+b;
	if(e>size)e=size;
	if(efd->num_w>=0)return 0;
	if(!(efd->run_w)){
		ev_io_start(EV_B_(evl) &(efd->io_w));
		efd->run_w = 1;
	}
	efd->num_w = dvm_sched_suspend(L);
	lua_pushlightuserdata(L,(void*)(data+b));
	lua_pushinteger(L,e-b);
	return lua_yield(L,6);
}

static int evio_FD_gc(lua_State* L){
	evio_rw* val = luaL_checkudata(L,1,METATABLE_FD);
	evloop evl = val->evl;
	if(val->run_r)
		ev_io_stop(EV_B_(evl) &(val->io_r));
	if(val->run_w)
		ev_io_stop(EV_B_(evl) &(val->io_w));
	return 0;
}

static int evio_FD_close(lua_State* L){
	evio_rw* val = luaL_checkudata(L,1,METATABLE_FD);
	close(val->io_r.fd);
	if(val->io_r.fd!=val->io_w.fd)
		close(val->io_w.fd);
	return 0;
}

static void evio_FD_mt(lua_State* L){
	if(luaL_newmetatable(L,METATABLE_FD)){
		int top = lua_gettop(L);
		lua_newtable(L);
		lua_pushcfunction(L,evio_FD_read);
		lua_setfield(L,top+1,"read");
		lua_pushcfunction(L,evio_FD_write);
		lua_setfield(L,top+1,"write");
		lua_pushcfunction(L,evio_FD_close);
		lua_setfield(L,top+1,"close");
		lua_setfield(L,top,"__index");
		lua_pushcfunction(L,evio_FD_gc);
		lua_setfield(L,top,"__gc");
		lua_settop(L,top);
	}
}

static int evio_wrap(lua_State* L){
	lua_settop(L,1);
	int fd = liolib_destroyfd(L,1);
	lua_settop(L,0);
	if(fd<0)
		lua_pushnil(L);
	else
		evio_pushfd(L,fd);
	return 1;
}

void evio_install(lua_State* L){
	lua_register(L,"nw_async_fd",evio_wrap);
}

void evio_pushfd(lua_State* L,int fd){
	setnonblock(fd);
	evio_rw* efd = lua_newuserdata(L,sizeof(evio_rw));
	efd->run_r=0;
	efd->run_w=0;
	efd->num_r=-1;
	efd->num_w=-1;
	evloop evl = evloop_get(L);
	efd->evl = evl;
	int top = lua_gettop(L);
	ev_io_init(&(efd->io_r),evio_io_r_cb,fd,EV_READ );
	ev_io_init(&(efd->io_w),evio_io_w_cb,fd,EV_WRITE);
	efd->io_r.data = efd;
	efd->io_w.data = efd;
	
	//ev_io_start(EV_B_(evl) &(efd->io_r));
	evio_FD_mt(L);
	lua_setmetatable(L,top);
}

int evio_getfd(lua_State* L,int index){
	ev_io *efd = luaL_testudata(L,index,METATABLE_FD);
	if(efd)return efd->fd;
	return -1;
}


