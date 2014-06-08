#include "rtlib.h"
#include "../lua/lauxlib.h"

static int lrtlib_end(lua_State* L){
	return lua_gettop(L);
}
static int lrtlib_try(lua_State* L){
	if(lua_gettop(L)==0){return 0;}
	return lua_pcallk(L,lua_gettop(L)-1,0,0,0,lrtlib_end);
}

static int lrtlib_undef(lua_State* L){
	lua_pushstring(L,"pcall/xpcall is not supported in iolua");
	lua_error(L);
	return 0;
}

void luartlib_install(lua_State* L){
	int top = lua_gettop(L);
	lua_register(L,"pcall",lrtlib_undef);
	lua_register(L,"xpcall",lrtlib_undef);
	lua_register(L,"try",lrtlib_try);
	lua_newtable(L);
	lua_setfield(L,LUA_REGISTRYINDEX,"rt.loaders");
	lua_newtable(L);
	lua_setfield(L,LUA_REGISTRYINDEX,"rt.modules");
	luaL_loadstring(L,
	"-- MODULE LOADER\n"
	"local modreg,ldrs=...\n"
	"modreg['$rt.modules']=modreg\n"
	"modreg['$rt.loaders']=ldrs\n"
	"function require(modname)"
	" if modreg[modname] then return modreg[modname] end\n"
	" local i=1\n"
	" repeat\n"
	"  if i>#ldrs then error('no such module: '..modname) end\n"
	"  ldrs[i](modname)\n"
	" until modreg[modname]\n"
	" return modreg[modname]\n"
	"end"
	);
	lua_getfield(L,LUA_REGISTRYINDEX,"rt.modules");
	lua_getfield(L,LUA_REGISTRYINDEX,"rt.loaders");
	lua_call(L,2,0);
	lua_getfield(L,LUA_REGISTRYINDEX,"rt.modules");
	luaopen_string(L);
	lua_setfield(L,top+1,"string");
	luaopen_table(L);
	lua_setfield(L,top+1,"table");
	luaopen_math(L);
	lua_setfield(L,top+1,"math");
	luaopen_bit32(L);
	lua_setfield(L,top+1,"bit32");
	luaopen_os(L);
	lua_setfield(L,top+1,"os");
	lua_settop(L,top);
}

