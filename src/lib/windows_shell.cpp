#include "stdafx.h"
#include "windows_shell.h"

#include <atlbase.h>
#include <shobjidl.h>
#include <shldisp.h>
#include <exdisp.h>
#include <shlguid.h>

BEG_LIB_NAMESPACE(windows_shell)

DECL_OBJECT(ShellFolderItemVerbObject)
{
public:
    ShellFolderItemVerbObject(CComPtr<FolderItemVerb> p)
        : _p(p)
    { }

    BEG_OBJ_API_GC(ShellFolderItemVerbObject, "windows_shell::ShellFolderItemVerbObject")
        OBJAPI(name)
        OBJAPI(doit)
    END_OBJ_API()

protected:
    LUAAPI(name)
    {
        DECL_THIS();
        CComBSTR bstrName;
        if((ComRet)O._p->get_Name(&bstrName)) {
            S.push((wchar_t*)bstrName);
        }
        else {
            S.push(L"");
        }
        return 1;
    }

    LUAAPI(doit)
    {
        DECL_THIS();
        S.push(O._p->DoIt());
        return 1;
    }

protected:
    CComPtr<FolderItemVerb> _p;
};

DECL_OBJECT(ShellFolderItemVerbsObject)
{
public:
    ShellFolderItemVerbsObject(CComPtr<FolderItemVerbs> p)
        : _p(p)
    { }

    BEG_OBJ_API_GC(ShellFolderItemVerbsObject, "windows_shell::ShellFolderItemVerbsObject")
        OBJAPI(count)
        OBJAPI(for_each)
    END_OBJ_API()

protected:
    LUAAPI(count)
    {
        DECL_THIS();
        long n = 0;
        O._p->get_Count(&n);
        S.push(n);
        return 1;
    }

    LUAAPI(for_each)
    {
        DECL_THIS();
        S.check_func(2);
        long n = 0;
        O._p->get_Count(&n);
        for(long i = 0; i < n; ++i) {
            CComVariant varIndex(i);
            CComPtr<FolderItemVerb> spVerb;
            if((ComRet)O._p->Item(varIndex, &spVerb)) {
                S.copy(2);
                S.push(i);
                S.push<ShellFolderItemVerbObject>(spVerb);
                S.call(2);
            }
        }
        return 0;
    }

protected:
    CComPtr<FolderItemVerbs> _p;
};

DECL_OBJECT(ShellFolderItemObject)
{
public:
    ShellFolderItemObject(CComPtr<FolderItem> p)
        : _p(p)
    { }

    BEG_OBJ_API_GC(ShellFolderItemObject, "windows_shell::ShellFolderItemObject")
        OBJAPI(verbs)
    END_OBJ_API()

protected:
    LUAAPI(verbs)
    {
        DECL_THIS();
        CComPtr<FolderItemVerbs> spVerbs;
        if((ComRet)O._p->Verbs(&spVerbs)) {
            S.push<ShellFolderItemVerbsObject>(spVerbs);
            return 1;
        }
        else {
            return 0;
        }
    }

protected:
    CComPtr<FolderItem> _p;
};

DECL_OBJECT(ShellFolderObject)
{
public:
    ShellFolderObject(CComPtr<Folder> p)
        : _p(p)
    { }

    BEG_OBJ_API_GC(ShellFolderObject, "windows_shell::ShellFolderObject")
        OBJAPI(parse_name)
    END_OBJ_API()

protected:
    LUAAPI(parse_name)
    {
        DECL_THIS();
        CComBSTR bstrName(S.check_str(2).c_str());
        CComPtr<FolderItem> spItem;
        if((ComRet)O._p->ParseName(bstrName, &spItem)) {
            S.push<ShellFolderItemObject>(spItem);
            return 1;
        }
        else {
            return 0;
        }
    }

protected:
    CComPtr<Folder> _p;
};

DECL_OBJECT(ShellDispatchObject)
{
public:
    ShellDispatchObject(IShellDispatch2* p)
        : _p(p)
    {}

    BEG_OBJ_API_GC(ShellDispatchObject, "windows_shell::ShellDispatchObject")
        OBJAPI2("namespace", namespace_)
    END_OBJ_API()

protected:
    LUAAPI(namespace_)
    {
        DECL_THIS();
        CComVariant varDir(S.check_str(2).c_str());
        CComPtr<Folder> spFolder;
        if((ComRet)O._p->NameSpace(varDir, &spFolder)) {
            S.push<ShellFolderObject>(spFolder);
            return 1;
        }
        else {
            return 0;
        }
    }

protected:
    CComPtr<IShellDispatch2> _p;
};

static void GetShellDispatch(IShellDispatch2** pp)
{
    CComPtr<IShellWindows> pSW;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&pSW)))) {
        HWND desktopHwnd = 0;
        CComPtr<IDispatch> pDisp;
        CComVariant vEmpty;
        if (SUCCEEDED(pSW->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, (long*)&desktopHwnd, SWFO_NEEDDISPATCH, &pDisp))) {
            if (desktopHwnd != NULL || desktopHwnd != INVALID_HANDLE_VALUE) {
                CComPtr<IShellBrowser> pSB;
                if (SUCCEEDED(IUnknown_QueryService(pDisp, SID_STopLevelBrowser, IID_PPV_ARGS(&pSB)))) {
                    CComPtr<IShellView> pSV;
                    if (SUCCEEDED(pSB->QueryActiveShellView(&pSV))) {
                        CComPtr<IDispatch> pDispBackground;
                        if (SUCCEEDED(pSV->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&pDispBackground)))) {
                            DWORD dwProcessId;
                            if (GetWindowThreadProcessId(desktopHwnd, &dwProcessId) && dwProcessId) {
                                AllowSetForegroundWindow(dwProcessId);
                            }
                            CComPtr<IShellFolderViewDual> pSFVD;
                            if (SUCCEEDED(pDispBackground->QueryInterface(IID_PPV_ARGS(&pSFVD)))) {
                                CComPtr<IDispatch> pDisp;
                                if (SUCCEEDED(pSFVD->get_Application(&pDisp))) {
                                    CComPtr<IShellDispatch2> pShellDispatch2;
                                    if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pShellDispatch2)))) {
                                        *pp = pShellDispatch2.Detach();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

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
    DECL_WRAP();
    MyStr lnk = S.check_str(1);
    MyStr src = S.check_str(2);
    MyStr arg = S.opt_str(3);
    MyStr ico = S.opt_str(4);
    ComRet hr = CreateShortcut(lnk, src, arg, ico);
    S.push(hr);
    return 1;
}

LUAAPI(shell_dispatch)
{
    DECL_WRAP();
    CComPtr<IShellDispatch2> spShellDispatch;
    GetShellDispatch(&spShellDispatch);
    if(spShellDispatch) {
        S.push<ShellDispatchObject>(spShellDispatch);
        return 1;
    }
    else {
        return 0;
    }
}

BEG_LIB_API()
    LIBAPI(create_shortcut)
    LIBAPI(shell_dispatch)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

END_LIB_NAMESPACE()
