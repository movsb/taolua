#include "stdafx.h"

#include "lua.h"
#include "libs.h"

using namespace taolua;

int wmain(int argc, wchar_t* argv[])
{
    int r;
    TaoLua lua;

    assert(argc == 2);

    lua.open();

    lua.exec(argv[1]);

    lua.close();

    return 0;
}
