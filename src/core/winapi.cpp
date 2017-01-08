#include "stdafx.h"
#include "winapi.h"

BEG_LIB_NAMESPACE(winapi)

LUAAPI(errno_)
{
    DECL_WRAP;
    G.getglobal("winapi");
    G.getfield("_errno");
    G.remove(-2);
    return 1;
}

LUAAPI(error)
{
    DECL_WRAP;
    wchar_t* buffer;
    DWORD code;
    if(G.isinteger(1)) {
        code = G.check_integer<DWORD>(1);
    }
    else {
        G.getglobal("winapi");
        G.getfield("_errno");
        code = G.check_integer<DWORD>(-1);
        G.pop(2);
    }

	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR)&buffer, 1, NULL))
	{
        G.push_fmt(L"(%d) %s", code, buffer);
		::LocalFree(buffer);
	}
    else {
        G.push(L"(winapi error: FormatMessage)");
    }

    return 1;
}

BEG_LIB_API()
    LIBAPI2("errno", errno_)
    LIBAPI(error)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{
    G.copy(-1);
    G.setglobal("winapi");
}

//////////////////////////////////////////////////////////////////////////

END_LIB_NAMESPACE()
