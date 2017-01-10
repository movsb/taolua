#include "stdafx.h"
#include "dialog.h"

BEG_LIB_NAMESPACE(dialog)

//////////////////////////////////////////////////////////////////////////

/*
    int msgbox(msg)
    int msgbox(msg, title)
    int msgbox(msg, icon)
    int msgbox(msg, title, icon)
*/
LUAAPI(msgbox)
{
    DECL_WRAP();
    std::wstring msg, title;
    UINT icon = MB_OK;

    msg = S.check_str(1);

    int n = S.gettop();
    if(n == 1) {

    }
    else if(n == 2) {
        if(S.isinteger(2))
            icon = S.tointeger<UINT>(2);
        else if(S.isstring(2))
            title = S.check_str(2);
        else
            S.argerr(2, "number or string expected");
    }
    else if(n == 3) {
        title = S.check_str(2);
        icon = S.check_integer<UINT>(3);
    }

    int ret = ::MessageBox(::GetActiveWindow(), msg.c_str(), title.c_str(), icon);

    S.push(ret);

    return 1;
}

static const wchar_t* _get_opensave_file(bool open, DWORD flags, LuaWrapper G)
{
    class CurDirKeeper
    {
    public:
        CurDirKeeper() { ::GetCurrentDirectory(_countof(_path), _path); }
        ~CurDirKeeper() { ::SetCurrentDirectory(_path); }
    protected:
        wchar_t _path[MAX_PATH];
    };

    static wchar_t path[MAX_PATH];
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hInstance = ::GetModuleHandle(nullptr);
    ofn.hwndOwner = ::GetActiveWindow();
    ofn.nMaxFile = _countof(path);
    ofn.lpstrFile = path;
    ofn.Flags = flags;
    path[0] = L'\0';

    // TODO: init from lua

    CurDirKeeper dirkeep;
    if(open) ::GetOpenFileName(&ofn);
    else ::GetSaveFileName(&ofn);
    return path;
}

LUAAPI(get_open_file)
{
    DECL_WRAP();
    DWORD flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST;
    auto path = _get_opensave_file(true, flags, S);
    S.push(path);
    return 1;
}

LUAAPI(get_save_file)
{
    DECL_WRAP();
    DWORD flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    auto path = _get_opensave_file(false, flags, S);
    S.push(path);
    return 1;
}

LUAAPI(choose_color)
{
    DECL_WRAP();
    static COLORREF last_colors[16];
    CHOOSECOLOR cc = {sizeof(cc)};

    cc.hwndOwner = ::GetActiveWindow();
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    cc.lpCustColors = last_colors;

    int R  = S.opt_int(1, 0x00);
    int G_ = S.opt_int(2, 0x00);
    int B  = S.opt_int(3, 0x00);
    COLORREF color = RGB(R, G_, B);
    cc.rgbResult = color;

    BoolVal ok = ::ChooseColor(&cc);
    S.push(ok && cc.rgbResult != color);

    if(ok) color = cc.rgbResult;

    S.push(color);
    S.push(GetRValue(color));
    S.push(GetGValue(color));
    S.push(GetBValue(color));

    return 5;
}

BEG_LIB_API()
    LIBAPI(msgbox)
    LIBAPI(get_open_file)
    LIBAPI(get_save_file)
    LIBAPI(choose_color)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

END_LIB_NAMESPACE()
