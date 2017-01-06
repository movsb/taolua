#include "stdafx.h"
#include "file_system.h"

namespace taolua {
namespace file_system {

class GC
{
public:
    GC() : i(9) {}

public:
    BEG_OBJ_API(GC, name)
        OBJAPI(__gc)
    END_OBJ_API()

    LIBAPI(__gc)
    {
        DECL_THIS;
        std::cout << "doing __gc: " << G.i << " ..." << std::endl;
        return 0;
    }

protected:
    int i;
};

LIBAPI(touch)
{
    DECL_WRAP;
    G.getglobal("print");
    G.push(1);
    G.push("432");
    G.push<GC>();
    G.push<GC>();
    G.call(4, 0);
    return 0;
}

BEG_LUA_API()
    LUAAPI(touch)
END_LUA_API()

}}
