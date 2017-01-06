#pragma once

namespace taolua
{

class TaoLua
{
public:
    TaoLua()
        : _L(nullptr)
    {}

public:
    // state
    void open();
    void close();

    // load
    int exec(const std::wstring& file);

protected:
    lua_State* _L;
};

}
