#include <taolua/taolua.h>
#include "window_manage.h"

TAOLUAAPI_INIT()
{
    args->newlib("window_manage", taolua::window_manage::__methods__, taolua::window_manage::__init__);
    return true;
}
