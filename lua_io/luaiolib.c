#include "luaiolib.h"
#include "../lua/lauxlib.h"
#include <stdlib.h>
#include <unistd.h>

#define METATABLE_FD "LUAIO_LIOLIB.FD"

#define MAXSIZE 1024

static int liolib_FD_read(lua_State* L){
	int *i = luaL_checkudata(L,1,METATABLE_FD);
	int s = lua_tointeger(L,2);
	int n;
	if(s<0)return 0;
	if(s>MAXSIZE) return 0;
	char content[s];
	n = read(*i,content,s);
	if(n>0){
		lua_pushlstring(L,content,n);
		return 1;
	}
	return 0;
}

static int liolib_FD_write(lua_State* L){
	const char* data;
	size_t size;
	lua_settop(L,4);
	int *i = luaL_checkudata(L,1,METATABLE_FD);
	int b=0,e=0;
	luaL_checktype(L,2,LUA_TSTRING);
	data = lua_tolstring(L,2,&size);
	e = size;
	if(lua_type(L,3)==LUA_TNUMBER)
		b = lua_tointeger(L,3);
	if(lua_type(L,4)==LUA_TNUMBER)
		e = lua_tointeger(L,4)+b;
	if(e>size)e=size;
	lua_pushinteger(L,write(*i,data+b,e-b));
	return 1;
}

static int liolib_FD_close(lua_State* L){
	int *i = lua_touserdata(L,1);
	if(i){
		if(*i>2) /* if *i != stdin, stdout and stderr */ close(*i);
		*i=-1;
	}
	return 0;
}

static void liolib_FD(lua_State* L){
	if(luaL_newmetatable(L,METATABLE_FD)){
		int top = lua_gettop(L);
		lua_newtable(L);
		lua_pushcfunction(L,liolib_FD_read);
		lua_setfield(L,top+1,"read");
		lua_pushcfunction(L,liolib_FD_write);
		lua_setfield(L,top+1,"write");
		lua_pushcfunction(L,liolib_FD_close);
		lua_setfield(L,top+1,"close");
		lua_setfield(L,top,"__index");
		lua_newtable(L);
		lua_setfield(L,top,"__metatable");
		lua_settop(L,top);
	}
}

void liolib_install(lua_State* L){
}

void liolib_pushfd(lua_State* L,int fd){
	int *i = (int*)lua_newuserdata(L,sizeof(int));
	int top = lua_gettop(L);
	*i=fd;
	liolib_FD(L);
	lua_setmetatable(L,top);
}

int liolib_getfd(lua_State* L,int index){
	int *i = (int*)luaL_testudata(L,index,METATABLE_FD);
	if(i)return *i;
	return -1;
}

int liolib_destroyfd(lua_State* L,int index){
	int fd=-1;
	int *i = (int*)luaL_testudata(L,index,METATABLE_FD);
	if(i){ fd = *i; *i=-1; }
	return fd;
}

