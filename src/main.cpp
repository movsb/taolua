#include "stdafx.h"

#include "lua.h"

#include "core/winapi.h"

#include <shlobj.h>

using namespace taolua;

static void assoc(bool yes)
{
    try {
        AutoRegistry hKey = ::RegCreateKey(HKEY_CLASSES_ROOT, L".tls", &hKey);
        if(!hKey) throw;

        std::wstring value(L"taolua.script");
        ::RegSetValueEx(hKey, L"", 0, REG_SZ, (BYTE const *)value.c_str(), string_bytes<DWORD>(value));

        AutoRegistry hKeyImp = ::RegCreateKey(HKEY_CLASSES_ROOT, value.c_str(), &hKeyImp);
        if(!hKeyImp) throw;

        std::wstring description(L"TaoLua Script File");
        ::RegSetValueEx(hKeyImp, L"", 0, REG_SZ, (BYTE const *)description.c_str(), string_bytes<DWORD>(description));

        AutoRegistry hKeyDefaultIcon = ::RegCreateKey(hKeyImp, L"Default", &hKeyDefaultIcon);
        if(!hKeyDefaultIcon) throw;

        wchar_t path[MAX_PATH]; path[0] = L'\0';
        ::GetModuleFileName(nullptr, path, _countof(path));
        std::wstring icon(path);
        ::RegSetValueEx(hKeyDefaultIcon, L"", 0, REG_SZ, (BYTE const *)icon.c_str(), string_bytes<DWORD>(icon));

        AutoRegistry hKeyShell = ::RegCreateKey(hKeyImp, L"shell\\open\\command", &hKeyShell);
        if(!hKeyShell) throw;

        std::wstring command(std::wstring(path) + L" %1");
        ::RegSetValueEx(hKeyShell, L"", 0, REG_SZ, (BYTE const *)command.c_str(), string_bytes<DWORD>(command));

        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH, 0, 0);
    }
    catch(...) {

    }
}

int wmain(int argc, wchar_t* argv[])
{
    if(argc != 2) return 0;

    if(argc == 2 && wcscmp(argv[1], L"--assoc") == 0) {
        assoc(true);
        return 0;
    }

    system("chcp 65001 > nul 2>&1");

    ::CoInitialize(nullptr);

    TaoLua lua;
    lua.open();

    auto G = &lua;

    MODULE(winapi);

    lua.exec(argv[1]);

    lua.close();

    ::CoUninitialize();

    return 0;
}
