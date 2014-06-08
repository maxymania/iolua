#include "evtp.h"
#include "lualoop_internals.h"
#include "../lua_tp/luatp.h"

#define FIELD "evtp_h.struct_regular_event"

#define TICKDELAY 0.01

struct regular_event{
	evloop evl;
	threadpool_t* pool;
	ev_timer t;
};

static void evtb_timer_cb(EV_P_ ev_timer *t, int revents){
	struct regular_event* r =t->data;
	threadpool_run_callbacks(r->pool);
	ev_timer_again(EV_A_ t);
}

void ev_tp_activate(lua_State* L){
	int top = lua_gettop(L);
	struct regular_event* r = lua_newuserdata(L,sizeof(struct regular_event));
	lua_setfield(L,LUA_REGISTRYINDEX,FIELD);
	lua_settop(L,top);
	r->evl = evloop_get(L);
	r->pool = luatp_pool(L);
	ev_timer_init(&(r->t),evtb_timer_cb,0.,TICKDELAY);
	r->t.data=r;
	ev_timer_again(EV_B_(r->evl) &(r->t));
}

