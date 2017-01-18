#include <taolua/taolua.h>
#include "file_system.h"

TAOLUAAPI_INIT()
{
    args->newlib("console", taolua::file_system::__methods__, taolua::file_system::__init__);
    return true;
}
