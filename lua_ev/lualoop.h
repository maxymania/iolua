#ifndef LUAEV_LLOOP
#define LUAEV_LLOOP
#include "../lua/lua.h"

struct evloop_s;
typedef struct evloop_s* evloop;

static evloop evloop_init(lua_State* L);

void evloop_install(lua_State* L);

evloop evloop_get(lua_State* L);

void evloop_run(evloop loop);

void evloop_unfreeze(evloop loop);

#endif

