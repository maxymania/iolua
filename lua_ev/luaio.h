#ifndef LUAEV_EVIO
#define LUAEV_EVIO
#include "../lua/lua.h"

void evio_install(lua_State* L);

void evio_pushfd(lua_State* L,int fd);

int evio_getfd(lua_State* L,int index);

#endif

