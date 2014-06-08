#ifndef LUAEV_EVBIO
#define LUAEV_EVBIO
#include "../lua/lua.h"

void evbio_install(lua_State* L);

void evbio_pushfd(lua_State* L,int fd);

int evbio_getfd(lua_State* L,int index);

#endif

