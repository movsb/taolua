#include "stdafx.h"

#include "lua.h"

namespace taolua
{

void TaoLua::open()
{
    _L = luaL_newstate();
    assert(_L);
    luaL_openlibs(_L);
}

void TaoLua::close()
{
    lua_close(_L);
    _L = nullptr;
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

    return luaL_dostring(_L, buf.get());
}

}
