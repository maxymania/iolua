#ifndef LUATP_H
#define LUATP_H
#include "../lua/lua.h"
#include "threadpool.h"

void luatp_init(lua_State* L,int maxthreads);

threadpool_t *luatp_pool(lua_State* L);

#endif

