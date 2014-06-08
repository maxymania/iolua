#include "scheduler.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"
#include "scheduler.lua.h"

static int canyield(lua_State* L){
	int top = lua_gettop(L);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__can_yield");
	int b = lua_toboolean(L,top+1);
	lua_settop(L,top);
	return b;
}

static void setcanyield(lua_State* L,int b){
	lua_pushboolean(L,b);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__can_yield");
}

static void dvm_threadhook (lua_State *L, lua_Debug *ar){
	if(!canyield(L)) return;
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__requeue");
	setcanyield(L,0);
	lua_call(L,0,0);
	setcanyield(L,1);
	lua_yield(L,0);
}

static int dvm_spawn (lua_State *L){
	int i=LUA_OK;
	if(lua_gettop(L)<1)return 0;
	if(!lua_isfunction(L,1))return 0;
	lua_State *O = lua_newthread(L);
	lua_sethook(O,dvm_threadhook,LUA_MASKCOUNT,256);
	lua_insert(L,1);
	lua_xmove(L,O,lua_gettop(L)-1);
	lua_resume(O,L,lua_gettop(O)-1);
	lua_settop(L,0);
	lua_pushboolean(L,LUA_OK==i);
	return 1;
}

static int dvm_yield (lua_State *L){
	lua_settop(L,0);
	return lua_yield(L,0);
}
static int dvm_thread(lua_State *L){
	lua_settop(L,0);
	lua_pushthread(L);
	return 1;
}
static int dvm_resume(lua_State *L){
	lua_settop(L,1);
	lua_State* x = lua_tothread(L,1);
	int i=LUA_OK;
	if(x){
		lua_settop(x,0);
		i=lua_resume(x,L,0);
	}
	lua_settop(L,0);
	lua_pushboolean(L,LUA_OK==i);
	return 1;
}
static int dvm_getruntime(lua_State *L){
	lua_rawgeti(L,LUA_REGISTRYINDEX,LUA_RIDX_MAINTHREAD);
	return 1;
}

void dvm_sched_init (lua_State* L){
	lua_pushboolean(L,1);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__can_yield");
	lua_pushcfunction(L,dvm_spawn);
	lua_setglobal(L,"spawn");
	lua_pushcfunction(L,dvm_yield);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__yield");
	lua_pushcfunction(L,dvm_thread);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__getthread");
	lua_pushcfunction(L,dvm_resume);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__runthread");
	lua_pushcfunction(L,dvm_getruntime);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__getruntime");
	luaL_loadstring(L,scheduler_lua);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__getthread");
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__runthread");
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__yield");
	lua_call(L,3,6);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.yield");
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__getsuspended");
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__wakeup");
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__suspend");
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__requeue");
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.__schedule");
	lua_settop(L,0);
}
int  dvm_sched_schedule(lua_State* L){
	int i = 0;
	lua_settop(L,0);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__schedule");
	lua_pcall(L,0,1,0);
	return lua_isthread(L,1);
}
void dvm_sched_wakeup  (lua_State* L,int i){
	lua_settop(L,0);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__wakeup");
	lua_pushinteger(L,i);
	lua_pcall(L,1,0,0);
}
lua_State* dvm_sched_getsuspended(lua_State* L,int i){
	lua_settop(L,0);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__getsuspended");
	lua_pushinteger(L,i);
	setcanyield(L,0);
	lua_pcall(L,1,1,0);
	setcanyield(L,1);
	lua_State* O = lua_tothread(L,1);
	lua_setfield(L,LUA_REGISTRYINDEX,"vm.  keepalive");
	lua_settop(L,0);
	return O;
}
int  dvm_sched_suspend (lua_State* L){
	lua_settop(L,0);
	lua_getfield(L,LUA_REGISTRYINDEX,"vm.__suspend");
	setcanyield(L,0);
	lua_call(L,0,1);
	setcanyield(L,1);
	int i = lua_tointeger(L,1);
	lua_settop(L,0);
	return i;
}

