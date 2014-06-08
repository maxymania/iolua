#include <stdlib.h>
#include "lualoop_internals.h"
#include "../lua/lauxlib.h"
#include "../luax/scheduler.h"

#define LOOPCONST "LUAEV_LLOOP.SRV"

#define TICKDELAY 0.01

static void sched_timer_cb(EV_P_ ev_timer *t, int revents) {
	evloop vm = t->data;
	if(dvm_sched_schedule(vm->L))
		ev_timer_again(EV_A_ t);
	else
		vm->on = 0;
}

void evloop_install(lua_State* L){
	evloop evl = malloc(sizeof(struct evloop_s));
	EV_LOOPINIT(evl,ev_default_loop(0));
	ev_timer_init(&evl->sched_timer, sched_timer_cb, 0., TICKDELAY);
	evl->L=L;
	evl->sched_timer.data=evl;
	evl->on = 1;
	ev_timer_again(EV_B_(evl) &(evl->sched_timer));
	lua_pushlightuserdata(L,evl);
	lua_setfield(L,LUA_REGISTRYINDEX,LOOPCONST);
}

evloop evloop_get(lua_State* L){
	lua_getfield(L,LUA_REGISTRYINDEX,LOOPCONST);
	evloop evl = lua_touserdata(L,lua_gettop(L));
	lua_pop(L,1);
	return evl;
}

void evloop_run(evloop loop){
	ev_loop( EV_B_(loop) 0);
}

void evloop_unfreeze(evloop loop){
	if(!loop->on)
		ev_timer_again(EV_B_(loop) &(loop->sched_timer));
	loop->on=1;
}

