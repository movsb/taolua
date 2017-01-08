#include "stdafx.h"
#include "windows_shell.h"

#include <atlbase.h>
#include <shobjidl.h>

BEG_LIB_NAMESPACE(windows_shell)

static ComRet CreateShortcut(LPCWSTR lnk, LPCWSTR src, LPCWSTR args, LPCWSTR icon)
{
    ComRet hr;
    CComPtr<IShellLink> spShelllink;

    hr = spShelllink.CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER);
    if(!hr) return hr;

    CComQIPtr<IPersistFile> spPersistFile(spShelllink);
    if(!spPersistFile) return E_NOINTERFACE;

    if(hr) hr = spShelllink->SetPath(src);
    if(hr) hr = spShelllink->SetShowCmd(SW_SHOWNORMAL);
    if(hr) hr = spShelllink->SetIconLocation(icon, 0);
    if(hr) hr = spShelllink->SetArguments(args);

    if(hr) hr = spPersistFile->Save(lnk, FALSE);

    return hr;
}

LUAAPI(create_shortcut)
{
    DECL_WRAP;
    MyStr lnk = G.check_str(1);
    MyStr src = G.check_str(2);
    MyStr arg = G.opt_str(3);
    MyStr ico = G.opt_str(4);
    ComRet hr = CreateShortcut(lnk, src, arg, ico);
    G.push(hr);
    return 1;
}

BEG_LIB_API()
    LIBAPI(create_shortcut)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

END_LIB_NAMESPACE()
