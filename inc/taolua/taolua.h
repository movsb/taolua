#pragma once

#include "stdafx.h"

#include "lua.h"

#define TAOLUAAPI extern "C" __declspec(dllexport)

#define TAOLUAAPI_INIT()      TAOLUAAPI bool taolua_init(taolua::TaoLuaInitParams* args)
#define TAOLUAAPI_UNINIT()    TAOLUAAPI bool taolua_uninit()

namespace taolua {
    struct TaoLuaInitParams
    {
        typedef void(*NEWLIB)(const char* name, const luaL_Reg fns[], void(*init)(taolua::LuaWrapper S));
        NEWLIB newlib;
    };
}
