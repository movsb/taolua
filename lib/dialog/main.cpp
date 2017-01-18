#include <taolua/taolua.h>
#include "dialog.h"

TAOLUAAPI_INIT()
{
    args->newlib("dialog", taolua::dialog::__methods__, taolua::dialog::__init__);
    return true;
}
