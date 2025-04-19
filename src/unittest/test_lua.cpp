// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
// Copyright (C) 2021 TurkeyMcMac, Jude Melton-Houghton <jwmhjwmh@gmail.com>

#include "test.h"
#include "config.h"
#include "catch.h"

#include <stdexcept>

extern "C" {
#if USE_LUAJIT
	#include <luajit.h>
#else
	#include <lua.h>
#endif
#include <lauxlib.h>
}

/*
 * Test for two common issues that prevent correct error handling
 * between Lua and C++.
 * Further reading:
 * - https://luajit.org/extensions.html#exceptions
 * - http://lua-users.org/wiki/ErrorHandlingBetweenLuaAndCplusplus
 */

TEST_CASE("Lua unwinds the stack correctly when it throws errors internally",
		// (This is not the case with PUC Lua unless it was compiled as C++.)
		"[lua-destructors]")
{
	class DestructorDetector {
		bool *did_destruct;
	public:
		DestructorDetector(bool *did_destruct) : did_destruct(did_destruct)
		{
			*did_destruct = false;
		}
		~DestructorDetector()
		{
			*did_destruct = true;
		}
	};

	bool did_destruct = false;

	lua_State *L = luaL_newstate();
	lua_cpcall(L, [](lua_State *L) -> int {
		DestructorDetector d(reinterpret_cast<bool*>(lua_touserdata(L, 1)));
		luaL_error(L, "error");
		return 0;
	}, &did_destruct);
	lua_close(L);

	CHECK(did_destruct);
}

static int wrapper(lua_State *L, lua_CFunction inner)
{
	try {
		return inner(L);
	} catch (std::exception &e) {
		lua_pushstring(L, e.what());
		return lua_error(L);
	}
}

TEST_CASE("C++ exceptions are caught and re-thrown as Lua errors",
		// This is handled by a wrapper we define ourselves.
		// (PUC Lua does not support use of such a wrapper, we have a patched version)
		"[lua-exceptions]")
{

	lua_State *L = luaL_newstate();

#if USE_LUAJIT
	lua_pushlightuserdata(L, reinterpret_cast<void*>(wrapper));
	luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
	lua_pop(L, 1);
#else
	lua_atccall(L, wrapper);
#endif

	lua_pushcfunction(L, [](lua_State *L) -> int {
		throw std::runtime_error("example");
	});

	int caught = 0;
	std::string errmsg;
	try {
		if (lua_pcall(L, 0, 0, 0) != 0) {
			caught = 2;
			errmsg = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
		}
	} catch (std::exception &e) {
		caught = 1;
	}

	if (caught != 1)
		lua_close(L);

	UASSERTEQ(int, caught, 2);
	UASSERT(errmsg.find("example") != std::string::npos);
}
