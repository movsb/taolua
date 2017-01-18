#include <taolua/taolua.h>
#include "process.h"

TAOLUAAPI_INIT()
{
    args->newlib("process", taolua::process::__methods__, taolua::process::__init__);
    return true;
}
