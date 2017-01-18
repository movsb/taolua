#include <taolua/taolua.h>
#include "console.h"

TAOLUAAPI_INIT()
{
    args->newlib("console", taolua::console::__methods__, taolua::console::__init__);
    return true;
}
