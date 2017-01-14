#include "stdafx.h"

#include "lua.h"

namespace taolua
{

void TaoLua::open()
{
    _L = luaL_newstate();
    assert(_L);
    luaL_openlibs(_L);

    _init_global();
}

void TaoLua::close()
{
    lua_close(_L);
    _L = nullptr;
}

void TaoLua::main()
{
    getglobal("taolua");
}

void TaoLua::newlib(const char* name, const luaL_Reg fns[], void(*init)(LuaWrapper G))
{
    main();
    newtable();
    setfuncs(fns);
    init(_L);
    setfield(-2, name);
    pop();
}

int TaoLua::exec(const std::wstring& file)
{
    std::ifstream f(file, std::ios::in | std::ios::binary);

    if(!f.is_open()) {
        printf("file not found: %s\n", to_utf8(file).c_str());
        return -1;
    }

    f.seekg(0, f.end);
    auto size = (size_t)f.tellg();
    auto buf = std::make_unique<char[]>(size + 1);
    f.seekg(f.beg);
    f.read(buf.get(), size);
    buf.get()[size] = '\0';

    int r = luaL_dostring(_L, buf.get());
    if(r != LUA_OK) {
        auto err = to_utf8(check_str(-1));
        printf("error executing lua: %s\n", err.c_str());
        pop();
    }

    return r;
}

int TaoLua::lua_exec(lua_State* L)
{
    DECL_WRAP;
    G.getglobal("taolua");
    G.getfield("__this__");
    auto __this = G.check_udata<TaoLua*>(-1);
    G.pop();
    auto file = G.check_str(1);
    auto r = __this->exec(file);
    G.push(r);
    return 1;
}

void TaoLua::_init_global()
{
    newtable();

    setfield("__this__", this);
    setfield("exec", lua_exec);

    setglobal("taolua");

    auto sleep = [](lua_State* L) -> int {
        ::Sleep((DWORD)luaL_checkinteger(L, 1));
        return 0;
    };

    push(sleep);
    setglobal("sleep");
}

}
