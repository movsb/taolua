#include <taolua/taolua.h>
#include "psapi.h"

TAOLUAAPI_INIT()
{
    args->newlib("psapi", taolua::psapi::__methods__, taolua::psapi::__init__);
    return true;
}
