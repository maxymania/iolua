#include "luatp.h"

static void noop(void *pp){}

static int tpgc(lua_State* L){
	threadpool_t** pool = lua_touserdata(L,1);
	int done;
	do {
		done = threadpool_die(*pool, 1);
		threadpool_run_callbacks(*pool);
	} while(!done);
	threadpool_destroy(*pool);
	*pool=NULL;
	return 0;
}

void luatp_init(lua_State* L,int maxthreads){
	int top = lua_gettop(L);
	threadpool_t** pool = lua_newuserdata(L,sizeof(threadpool_t*));
	*pool = threadpool_create(maxthreads,noop,NULL);
	lua_newtable(L);
	lua_pushcfunction(L,tpgc);
	lua_setfield(L,top+2,"__gc");
	lua_setmetatable(L,top+1);
	lua_setfield(L,LUA_REGISTRYINDEX,"luatp.threadpool");
	lua_settop(L,top);
}

threadpool_t *luatp_pool(lua_State* L){
	threadpool_t** pool;
	threadpool_t* vpool;
	int top = lua_gettop(L);
	lua_getfield(L,LUA_REGISTRYINDEX,"luatp.threadpool");
	pool = lua_touserdata(L,top+1);
	vpool = pool?*pool:NULL;
	lua_settop(L,top);
	return vpool;
}

