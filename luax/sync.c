#include "sync.h"
#include "../lua/lauxlib.h"

#define LSYNC_ATOMIC "LSYNC_H.ATOMIC"


static int lsync_getandset(lua_State* L){
	int *i = luaL_checkudata(L,1,LSYNC_ATOMIC);
	int j=*i;
	*i = lua_toboolean(L,2);
	lua_settop(L,0);
	lua_pushboolean(L,j);
	return 1;
}


static void lsync_MT(lua_State* L){
	if(luaL_newmetatable(L,LSYNC_ATOMIC)){
		int top = lua_gettop(L);
		lua_pushcfunction(L,lsync_getandset);
		lua_setfield(L,top,"__pow");
		lua_pushcfunction(L,lsync_getandset);
		lua_setfield(L,top,"__mul");
		lua_newtable(L);
		lua_setfield(L,top,"__metatable");
		lua_settop(L,top);
	}
}

static int lsync_new(lua_State* L){
	lua_settop(L,0);
	int *i = lua_newuserdata(L,sizeof(int));
	*i=0;
	lsync_MT(L);
	lua_setmetatable(L,1);
	return 1;
}

void lsync_install(lua_State* L){
	int top = lua_gettop(L);
	lua_getfield(L,LUA_REGISTRYINDEX,"rt.modules");
	lua_newtable(L);
	lua_pushcfunction(L,lsync_new);
	lua_setfield(L,top+2,"lock");
	lua_setfield(L,top+1,"sync");
	lua_settop(L,top);
}

