#include <taolua/stdafx.h>
#include "demo.h"

namespace taolua {
namespace demo {

LUAAPI(test)
{
    DECL_WRAP;
    G.push(L"string returned from demo::test()");
    return 1;
}

BEG_LIB_API()
    LIBAPI(test)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

} }
