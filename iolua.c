#include <stdio.h>
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "luax/scheduler.h"
#include "luax/rtlib.h"
#include "luax/sync.h"
#include "lua_io/luaiolib.h"
#include "lua_io/luanetlib.h"
#include "lua_io/lua_file.h"
#include "lua_tp/luatp.h"
#include "lua_ev/lualoop.h"
#include "lua_ev/evtp.h"
#include "lua_ev/luaio.h"
#include "lua_ev/luanet.h"
#include "lua_ev/luabio.h"

int main(int argc,const char** argv){
	int i,j;
	if(argc>1){
	lua_State* L = luaL_newstate();
	luaopen_base(L);
	lua_settop(L,0);
	luatp_init(L,10);
	luartlib_install(L);
	lsync_install(L);
	liolib_install(L);
	netlib_install(L);
	liofile_install(L);
	evloop_install(L);
	ev_tp_activate(L);
	evio_install(L);
	evnet_install(L);
	evbio_install(L);
	
	lua_pop(L,1);
	lua_settop(L,0);
	dvm_sched_init(L);
	
	//liolib_pushfd(L,0);
	evbio_pushfd(L,0);
	lua_setglobal(L,"STDIN");
	evbio_pushfd(L,1);
	lua_setglobal(L,"STDOUT");
	evbio_pushfd(L,2);
	lua_setglobal(L,"STDERR");
	
	
	lua_getglobal(L,"spawn");
	luaL_loadfile(L,argv[1]);
	lua_pcall(L,1,0,0);
	lua_settop(L,0);
	evloop_run(evloop_get(L));
	
	lua_close(L);
	}return 1;
}

