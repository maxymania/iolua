
#ifndef LUAEV_LLOOP_I
#define LUAEV_LLOOP_I
#include "lualoop.h"
#include "../libev/ev.h"

#if EV_MULTIPLICITY
# define EV_FIELD struct ev_loop* loop;
# define EV_B(x) x->loop
# define EV_B_(x) x->loop,
# define EV_LOOPINIT(x,i) x->loop=i
#else
# define EV_FIELD
# define EV_B(x) 
# define EV_B_(x)
# define EV_LOOPINIT(x,i) i
#endif

struct evloop_s{
	lua_State* L;
	struct ev_timer sched_timer;
	EV_FIELD
	int on;
};

#endif

