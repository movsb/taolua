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

void TaoLua::newlib(const char* name, const luaL_Reg fns[])
{
    main();
    newtable();
    setfuncs(fns);
    setfield(-2, name);
    pop();
}

int TaoLua::exec(const std::wstring& file)
{
    std::ifstream f(file, std::ios::in | std::ios::binary);
    assert(f.is_open());

    f.seekg(0, f.end);
    auto size = (size_t)f.tellg();
    auto buf = std::make_unique<char[]>(size + 1);
    f.seekg(f.beg);
    f.read(buf.get(), size);
    buf.get()[size] = '\0';

    int r = luaL_dostring(_L, buf.get());
    if(r != LUA_OK) {
        std::cout << lua_tostring(_L, -1);
        pop();
    }

    return r;
}

void TaoLua::_init_global()
{
    newtable();
    setglobal("taolua");
}

}
