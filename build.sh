#!/bin/sh

LIBEV_A=libev/.libs/libev.a

pushd lua
	cc -c *.c
popd
pushd luax
	#VARNAME=scheduler_lua ../f2chc.pl scheduler.lua > scheduler.lua.h
	cc -c *.c
popd
pushd lua_io
	cc -c *.c
popd
pushd lua_tp
	cc -c *.c
popd
pushd lua_ev
	cc -c *.c
popd
pushd nwlib
	cc -c *.c
popd

#echo $LIBEV_A
cc -c iolua.c
cc -o iolua iolua.o lua/*.o luax/*.o lua_io/*.o lua_ev/*.o lua_tp/*.o nwlib/*.o $LIBEV_A -lpthread
