#include "lua_file.h"
#include "../lua/lauxlib.h"
#include "../lua_ev/luabio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// #define FLAG |O_LARGEFILE

static int liofile_create(lua_State* L){
	lua_settop(L,2);
	const char* filename = luaL_checkstring(L,1);
	mode_t mode = lua_tointeger(L,2);
	if(!mode)mode=0666;
	int fd = open(filename,O_CREAT|O_TRUNC|O_RDWR,mode);
	if(fd<0)return 0;
	evbio_pushfd(L,fd);
	return 1;
}
static int liofile_read(lua_State* L){
	lua_settop(L,2);
	const char* filename = luaL_checkstring(L,1);
	mode_t mode = lua_tointeger(L,2);
	if(!mode)mode=0666;
	int fd = open(filename,O_RDONLY,mode);
	if(fd<0)return 0;
	evbio_pushfd(L,fd);
	return 1;
}
static int liofile_open(lua_State* L){
	lua_settop(L,2);
	const char* filename = luaL_checkstring(L,1);
	mode_t mode = lua_tointeger(L,2);
	if(!mode)mode=0666;
	int fd = open(filename,O_CREAT|O_RDWR,mode);
	if(fd<0)return 0;
	evbio_pushfd(L,fd);
	return 1;
}

void liofile_install(lua_State* L){
	lua_register(L,"fileCreate",liofile_create);
	lua_register(L,"fileRead",liofile_read);
	lua_register(L,"fileOpen",liofile_open);
}

