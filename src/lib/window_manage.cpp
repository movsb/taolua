#include "stdafx.h"
#include "window_manage.h"

namespace taolua {
namespace window_manage {

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
    END_OBJ_API()

    LIBAPI(__tostring)
    {
        DECL_THIS;
        G.push_fmt(L"%s { hWnd: 0x%08X }", __namew__(), O._hwnd);
        return 1;
    }

    LIBAPI(hwnd)
    {
        DECL_THIS;
        G.push(O._hwnd);
        return 1;
    }

    LIBAPI(show)
    {
        DECL_THIS;
        auto cmd = G.opt_int(1, SW_SHOW);
        ::ShowWindow(O._hwnd, cmd);
        return 0;
    }

    LIBAPI(hide)
    {
        DECL_THIS;
        ::ShowWindow(O._hwnd, SW_HIDE);
        return 0;
    }

    LIBAPI(class_name)
    {
        DECL_THIS;
        wchar_t cls[MAX_PATH];
        cls[0] = L'\0';
        ::GetClassName(O._hwnd, cls, _countof(cls));
        G.push(cls);
        return 1;
    }

    LIBAPI(title)
    {
        DECL_THIS;
        size_t len = ::GetWindowTextLength(O._hwnd);
        auto str = std::make_unique<wchar_t[]>(len + 1);
        str.get()[0] = str.get()[len] = L'\0';
        ::GetWindowText(O._hwnd, str.get(), len + 1);
        G.push(str.get());
        return 1;
    }

protected:
    HWND _hwnd;
};

//////////////////////////////////////////////////////////////////////////

LIBAPI(towinobj)
{
    DECL_WRAP;
    auto wnd = G.check_udata<HWND>(1);
    G.push<WindowObject>(wnd);
    return 1;
}

LIBAPI(get_active_window)
{
    DECL_WRAP;
    G.push(::GetActiveWindow());
    return 1;
}

LIBAPI(get_foreground_window)
{
    DECL_WRAP;
    G.push(::GetForegroundWindow());
    return 1;
}

LIBAPI(find_window)
{
    DECL_WRAP;
    auto title = G.opt_str(1, L"t");
    auto cls = G.opt_str(2, nullptr);

    HWND hWnd = ::FindWindow(cls, title);
    G.push(hWnd);

    return 1;
}

BEG_LUA_API()
    LUAAPI(towinobj)
    LUAAPI(get_active_window)
    LUAAPI(get_foreground_window)
    LUAAPI(find_window)
END_LUA_API()

//////////////////////////////////////////////////////////////////////////

}
}
