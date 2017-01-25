#include <taolua/taolua.h>
#include "json.h"

TAOLUAAPI_INIT()
{
    args->newlib("json", taolua::json::__methods__, taolua::json::__init__);
    return true;
}
