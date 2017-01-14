#include "stdafx.h"
#include "window_manage.h"

// BEG_LIB_NAMESPACE(window_manage)
namespace taolua { namespace window_manage {

class Rect : public RECT
{
public:
    Rect()
    {
        ::memset(this, 0, sizeof(RECT));
    }

    LONG width() const { return right - left; }
    LONG height() const { return bottom - top; }
};

DECL_OBJECT(WindowObject)
{
public:
    WindowObject(HWND hWnd)
        :_hwnd(hWnd)
    { }

    BEG_OBJ_API(window_manage, WindowObject)
        OBJAPI(__tostring)
        OBJAPI(hwnd)
        OBJAPI(show)
        OBJAPI(hide)
        OBJAPI(close)
        OBJAPI2("class", class_name)
        OBJAPI(title)
        OBJAPI2("text", title)
        OBJAPI(sendmsg)
        OBJAPI(postmsg)
        OBJAPI(term)
        OBJAPI(top)
        OBJAPI(pid)
        OBJAPI(tid)
        OBJAPI(visible)
        OBJAPI(size)
        OBJAPI(pos)
        OBJAPI(band)
    END_OBJ_API()

    LUAAPI(__tostring)
    {
        DECL_THIS;
        G.push_fmt(L"%s { hWnd: 0x%08X }", __namew__(), O._hwnd);
        return 1;
    }

    LUAAPI(visible)
    {
        DECL_THIS;
        BoolVal br = ::IsWindowVisible(O._hwnd);
        G.push(br);
        return 1;
    }

    LUAAPI(hwnd)
    {
        DECL_THIS;
        G.push_as_int(O._hwnd);
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

    LUAAPI(close)
    {
        DECL_THIS;
        auto use_post = G.opt_bool(2, true);
        if(use_post) ::PostMessage(O._hwnd, WM_CLOSE, 0, 0);
        else ::SendMessage(O._hwnd, WM_CLOSE, 0, 0);
        G.push(!::IsWindow(O._hwnd));
        return 1;
    }

    LUAAPI(term)
    {
        DECL_THIS;
        DWORD pid; ::GetWindowThreadProcessId(O._hwnd, &pid);
        AutoHandle hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if(hProcess) {
            BoolVal br = ::TerminateProcess(hProcess, -1);
            SAVE_LAST_ERROR;
            G.push(br);
            return 1;
        }
        else {
            SAVE_LAST_ERROR;
            G.push(false);
            return 1;
        }
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

    LUAAPI(top)
    {
        DECL_THIS;
        if(!G.isnone(2)) {
            BoolVal br = G.check_bool(2);
            ::SetWindowPos(O._hwnd, br ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            SAVE_LAST_ERROR;
        }

        auto style = ::GetWindowLongPtr(O._hwnd, GWL_EXSTYLE);
        BoolVal isTop = style != 0 && style & WS_EX_TOPMOST;

        G.push(isTop);
        return 1;
    }

    LUAAPI(pid)
    {
        DECL_THIS;
        DWORD id;
        ::GetWindowThreadProcessId(O._hwnd, &id);
        G.push(id);
        return 1;
    }

    LUAAPI(tid)
    {
        DECL_THIS;
        DWORD id = ::GetWindowThreadProcessId(O._hwnd, nullptr);
        G.push(id);
        return 1;
    }

    LUAAPI(size)
    {
        DECL_THIS;
        Rect rc;
        ::GetWindowRect(O._hwnd, &rc);
        G.push(rc.width());
        G.push(rc.height());
        return 2;
    }

    LUAAPI(pos)
    {
        DECL_THIS;
        Rect rc;
        ::GetWindowRect(O._hwnd, &rc);
        G.push(rc.left);
        G.push(rc.top);
        G.push(rc.right);
        G.push(rc.bottom);
        return 4;
    }

    LUAAPI(band)
    {
        DECL_THIS;
        typedef BOOL (WINAPI *GetWindowBandT)(HWND hWnd, PDWORD pdwBand);
        static auto fn = (GetWindowBandT)::GetProcAddress(::GetModuleHandle(L"user32.dll"), "GetWindowBand");
        if(fn) {
            DWORD band = 0;
            BoolVal br = fn(O._hwnd, &band);
            G.push(br ? band : 0);
            return 1;
        }
        else {
            G.push(0);
            return 1;
        }
    }

protected:
    HWND _hwnd;
};

//////////////////////////////////////////////////////////////////////////

LUAAPI(towinobj)
{
    DECL_WRAP;
    auto wnd = G.check_integer<HWND>(1);
    if(::IsWindow(wnd))
        G.push<WindowObject>(wnd);
    else
        G.push();
    return 1;
}

LUAAPI(get_active_window)
{
    DECL_WRAP;
    G.push_as_int(::GetActiveWindow());
    return 1;
}

LUAAPI(get_foreground_window)
{
    DECL_WRAP;
    G.push_as_int(::GetForegroundWindow());
    return 1;
}

LUAAPI(find_window)
{
    DECL_WRAP;
    auto title = G.opt_str(1, nullptr);
    auto cls = G.opt_str(2, nullptr);

    HWND hWnd = ::FindWindow(cls, title);
    G.push_as_int(hWnd);

    return 1;
}

LUAAPI(enum_windows)
{
    DECL_WRAP;
    // BOOL CALLBACK EnumWindowsProc( _In_ HWND   hwnd, _In_ LPARAM lParam);
    static auto EnumWindowsProc = [](HWND hwnd, LPARAM lparam) -> BOOL
    {
        auto G = (LuaWrapper*)lparam;
        G->copy(-1);
        G->push_as_int(hwnd);

        bool bContinue = true;

        G->call(1, 1);
        bContinue = G->opt_bool(-1, true);
        G->pop();

        return bContinue;
    };

    G.check_func(1);

    ::EnumWindows(EnumWindowsProc, (LPARAM)&G);

    return 0;
}

BEG_LIB_API()
    LIBAPI(towinobj)
    LIBAPI(get_active_window)
    LIBAPI(get_foreground_window)
    LIBAPI(find_window)
    LIBAPI(enum_windows)
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

    G.push(winmsg);
    G.setfield(-2, "winmsg");
}

//////////////////////////////////////////////////////////////////////////

END_LIB_NAMESPACE()
