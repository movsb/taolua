#include "stdafx.h"
#include "window_manage.h"

BEG_LIB_NAMESPACE(window_manage)

DECL_OBJECT(WindowObject)
{
public:
    WindowObject(HWND hWnd)
        :_hwnd(hWnd)
    { }

    BEG_OBJ_API(WindowObject, "window_manage::WindowObject")
        OBJAPI(__tostring)
        OBJAPI(hwnd)
        OBJAPI(show)
        OBJAPI(hide)
        OBJAPI2("class", class_name)
        OBJAPI(title)
        OBJAPI(sendmsg)
        OBJAPI(postmsg)
    END_OBJ_API()

    LUAAPI(__tostring)
    {
        DECL_THIS;
        G.push_fmt(L"%s { hWnd: 0x%08X }", __namew__(), O._hwnd);
        return 1;
    }

    LUAAPI(hwnd)
    {
        DECL_THIS;
        G.push(O._hwnd);
        return 1;
    }

    LUAAPI(show)
    {
        DECL_THIS;
        auto cmd = G.opt_int(2, SW_SHOW);
        ::ShowWindow(O._hwnd, cmd);
        return 0;
    }

    LUAAPI(hide)
    {
        DECL_THIS;
        ::ShowWindow(O._hwnd, SW_HIDE);
        return 0;
    }

    LUAAPI(class_name)
    {
        DECL_THIS;
        wchar_t cls[MAX_PATH];
        cls[0] = L'\0';
        ::GetClassName(O._hwnd, cls, _countof(cls));
        G.push(cls);
        return 1;
    }

    LUAAPI(title)
    {
        DECL_THIS;
        size_t len = ::GetWindowTextLength(O._hwnd);
        auto str = std::make_unique<wchar_t[]>(len + 1);
        str.get()[0] = str.get()[len] = L'\0';
        ::GetWindowText(O._hwnd, str.get(), (int)(len + 1));
        G.push(str.get());
        return 1;
    }

    LUAAPI(sendmsg)
    {
        DECL_THIS;
        auto msg = (UINT)G.check_int(2);
        auto ty3 = G.type(3), ty4 = G.type(4);
        std::wstring str3, str4;
        WPARAM wp = 0; LPARAM lp = 0;

        if(ty3 == LUA_TNUMBER)      wp = G.check_int(3);
        else if(ty3 == LUA_TSTRING) { str3 = G.check_str(3); wp = (WPARAM)str3.c_str(); }
        else if(ty3 == LUA_TNONE) {}
        else G.argerr(3, "number or string expected");

        if(ty4 == LUA_TNUMBER)      lp = G.check_int(4);
        else if(ty4 == LUA_TSTRING) { str4 = G.check_str(4); lp = (LPARAM)str4.c_str(); }
        else if(ty4 == LUA_TNONE) {}
        else G.argerr(4, "number or string expected");

        LRESULT ret = ::SendMessage(O._hwnd, msg, wp, lp);

        G.push(lua_Integer(ret));

        return 1;
    }

    LUAAPI(postmsg)
    {
        DECL_THIS;
        auto msg = (UINT)G.check_int(2);
        auto ty3 = G.type(3), ty4 = G.type(4);
        std::wstring str3, str4;
        WPARAM wp = 0; LPARAM lp = 0;

        if(ty3 == LUA_TNUMBER)      wp = G.check_int(3);
        else if(ty3 == LUA_TSTRING) { str3 = G.check_str(3); wp = (WPARAM)str3.c_str(); }
        else if(ty3 == LUA_TNONE) {}
        else G.argerr(3, "number or string expected");

        if(ty4 == LUA_TNUMBER)      lp = G.check_int(4);
        else if(ty4 == LUA_TSTRING) { str4 = G.check_str(4); lp = (LPARAM)str4.c_str(); }
        else if(ty4 == LUA_TNONE) {}
        else G.argerr(4, "number or string expected");

        int ret = ::PostMessage(O._hwnd, msg, wp, lp);

        G.push(ret);

        return 1;
    }

protected:
    HWND _hwnd;
};

//////////////////////////////////////////////////////////////////////////

LUAAPI(towinobj)
{
    DECL_WRAP;
    auto wnd = G.check_udata<HWND>(1);
    G.push<WindowObject>(wnd);
    return 1;
}

LUAAPI(get_active_window)
{
    DECL_WRAP;
    G.push(::GetActiveWindow());
    return 1;
}

LUAAPI(get_foreground_window)
{
    DECL_WRAP;
    G.push(::GetForegroundWindow());
    return 1;
}

LUAAPI(find_window)
{
    DECL_WRAP;
    auto title = G.opt_str(1, L"t");
    auto cls = G.opt_str(2, nullptr);

    HWND hWnd = ::FindWindow(cls, title);
    G.push(hWnd);

    return 1;
}

BEG_LIB_API()
    LIBAPI(towinobj)
    LIBAPI(get_active_window)
    LIBAPI(get_foreground_window)
    LIBAPI(find_window)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{
#define _ STRINTPAIR
    static CStr2Int winmsg {
        _(WM_NULL)
        _(WM_CREATE)
        _(WM_DESTROY)
        _(WM_MOVE)
        _(WM_SIZE)
        _(WM_ACTIVATE)
        _(WM_SETFOCUS)
        _(WM_KILLFOCUS)
        _(WM_ENABLE)
        _(WM_SETTEXT)
        _(WM_GETTEXT)
        _(WM_GETTEXTLENGTH)
        _(WM_PAINT)
        _(WM_CLOSE)
        _(WM_QUIT)
        _(WM_ACTIVATEAPP)
    };
#undef _

    G.newtable(winmsg);
    G.setfield(-2, "winmsg");
}

//////////////////////////////////////////////////////////////////////////

END_LIB_NAMESPACE()
