#include "stdafx.h"

#include "lua.h"

#include "core/winapi.h"

#include "lib/file_system.h"
#include "lib/window_manage.h"
#include "lib/psapi.h"
#include "lib/dialog.h"
#include "lib/windows_shell.h"

using namespace taolua;

int wmain(int argc, wchar_t* argv[])
{
    TaoLua lua;

    assert(argc == 2);

    system("chcp 65001 > nul 2>&1");

    ::CoInitialize(nullptr);

    lua.open();

    MODULE(winapi);

    MODULE(file_system);
    MODULE(window_manage);
    MODULE(psapi);
    MODULE(dialog);
    MODULE(windows_shell);

    lua.exec(argv[1]);

    lua.close();

    ::CoUninitialize();

    return 0;
}
