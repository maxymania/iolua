#include <stdio.h>
#include <stdlib.h>
#include "luabio.h"
#include "lualoop_internals.h"
#include <unistd.h>
#include "../lua/lauxlib.h"
#include "../luax/scheduler.h"
#include "../lua_tp/luatp.h"

#define METATABLE_FD "LUAEV_BIO.FD"
#define MAXSIZE 4096

typedef struct{
	ev_async a;
	evloop evl;
	threadpool_t* pool;
	int active;
	int fd;
	int num;
	void* data;
	int n;
} evbio_io;

static void evbio_read_cb (void* a);
static void evbio_read_run(void* ptr){
	evbio_io* io = ptr;
	io->n = read(io->fd,io->data,io->n);
	threadpool_schedule_back(io->pool,evbio_read_cb,ptr);
}
static void evbio_read_cb (void* a){
	evbio_io* io = (evbio_io*)a;
	evloop evl = io->evl;
	io->active = 0;
	lua_State* D = (io->num<0)?NULL:dvm_sched_getsuspended(evl->L,io->num);
	io->num = -1;
	if(!D)return;
	if((io->n)>0)
		lua_pushlstring(D,io->data,io->n);
	else
		lua_pushnil(D);
	free(io->data);
	lua_replace(D,1);
	lua_settop(D,1);
	// We have still 4 elements on the stack but we will use just the upperst one
	lua_resume(D,evl->L,1);
	//lua_settop(D,0);
	evloop_unfreeze(evl);
}
static int evbio_FD_read(lua_State* L){
	lua_settop(L,2);
	evbio_io* efd = luaL_checkudata(L,1,METATABLE_FD);
	evloop evl = efd->evl;
	threadpool_t* pool = efd->pool;
	int s = lua_tointeger(L,2);
	if(s<=0)return 0;
	if(s>MAXSIZE) return 0;
	if(efd->num>=0)return 0;
	if(efd->active)return 0;
	efd->data = malloc(s);
	if(efd->data==NULL)return 0;
	efd->active=1;
	efd->num = dvm_sched_suspend(L);
	efd->n = s;
	threadpool_schedule(pool,evbio_read_run,efd);
	return lua_yield(L,1);
}


static void evbio_write_cb (void* a){
	evbio_io* io = (evbio_io*)a;
	evloop evl = io->evl;
	io->active = 0;
	lua_State* D = (io->num<0)?NULL:dvm_sched_getsuspended(evl->L,io->num);
	io->num = -1;
	if(!D)return;
	lua_pushinteger(D,io->n);
	lua_replace(D,1);
	lua_settop(D,1);
	lua_resume(D,evl->L,1);
	evloop_unfreeze(evl);
}
static void evbio_write_run(void* ptr){
	evbio_io* io = ptr;
	io->n = write(io->fd,io->data,io->n);
	threadpool_schedule_back(io->pool,evbio_write_cb,ptr);
}
static int evbio_FD_write(lua_State* L){
	const char* data;
	size_t size;
	lua_settop(L,4);
	evbio_io* efd = luaL_checkudata(L,1,METATABLE_FD);
	threadpool_t* pool = efd->pool;
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
	if(efd->num>=0)return 0;
	if(efd->active)return 0;
	efd->active=1;
	efd->num = dvm_sched_suspend(L);
	efd->data = (void*)(data+b);
	efd->n = e-b;
	threadpool_schedule(pool,evbio_write_run,efd);
	return lua_yield(L,4);
}

static int evbio_FD_close(lua_State* L){
	evbio_io* efd= lua_touserdata(L,1);
	if(efd){
		if((efd->fd)>2) /* if *i != stdin, stdout and stderr */ close(efd->fd);
		efd->fd=-1;
	}
	return 0;
}

static void evbio_FD(lua_State* L){
	if(luaL_newmetatable(L,METATABLE_FD)){
		int top = lua_gettop(L);
		lua_newtable(L);
		lua_pushcfunction(L,evbio_FD_read);
		lua_setfield(L,top+1,"read");
		lua_pushcfunction(L,evbio_FD_write);
		lua_setfield(L,top+1,"write");
		lua_pushcfunction(L,evbio_FD_close);
		lua_setfield(L,top+1,"close");
		lua_setfield(L,top,"__index");
		lua_settop(L,top);
	}
}

void evbio_install(lua_State* L){}

void evbio_pushfd(lua_State* L,int fd){
	evbio_io *io = lua_newuserdata(L,sizeof(evbio_io));
	int top = lua_gettop(L);
	io->evl = evloop_get(L);
	io->pool = luatp_pool(L);
	io->active=0;
	io->num=-1;
	io->fd = fd;
	evbio_FD(L);
	lua_setmetatable(L,top);
	lua_settop(L,top);
}

int evbio_getfd(lua_State* L,int index){
	evbio_io *io = luaL_testudata(L,index,METATABLE_FD);
	return io?io->fd:-1;
}

