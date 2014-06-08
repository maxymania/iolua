#ifndef LUAIO_LIOLIB
#define LUAIO_LIOLIB
#include "../lua/lua.h"

void liolib_install(lua_State* L);

void liolib_pushfd(lua_State* L,int fd);

int liolib_getfd(lua_State* L,int index);

/* destroys the fd object, returning the fd */
int liolib_destroyfd(lua_State* L,int index);

#endif

