#ifndef VM_SCHED_H
#define VM_SCHED_H 1
#include "../lua/lua.h"

void dvm_sched_init    (lua_State* L);
int  dvm_sched_schedule(lua_State* L);
void dvm_sched_wakeup  (lua_State* L,int i);
lua_State* dvm_sched_getsuspended(lua_State* L,int i);
int  dvm_sched_suspend (lua_State* L);

#endif

