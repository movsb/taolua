#include <taolua/taolua.h>
#include "windows_shell.h"

TAOLUAAPI_INIT()
{
    args->newlib("windows_shell", taolua::windows_shell::__methods__, taolua::windows_shell::__init__);
    return true;
}
