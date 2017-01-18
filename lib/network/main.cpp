#include <taolua/taolua.h>
#include "network.h"

TAOLUAAPI_INIT()
{
    args->newlib("network", taolua::network::__methods__, taolua::network::__init__);
    return true;
}
