#include <taolua/taolua.h>
#include "demo.h"

TAOLUAAPI_INIT()
{
    args->newlib("demo", taolua::demo::__methods__, taolua::demo::__init__);
    return true;
}
