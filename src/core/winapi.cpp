#include "stdafx.h"
#include "winapi.h"

BEG_LIB_NAMESPACE(winapi)

LUAAPI(errno_)
{
    DECL_WRAP();
    S.getglobal("winapi");
    S.getfield("_errno");
    S.remove(-2);
    return 1;
}

LUAAPI(error)
{
    DECL_WRAP();
    wchar_t* buffer;
    DWORD code;
    if(S.isinteger(1)) {
        code = S.check_integer<DWORD>(1);
    }
    else {
        S.getglobal("winapi");
        S.getfield("_errno");
        code = S.check_integer<DWORD>(-1);
        S.pop(2);
    }

	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR)&buffer, 1, NULL))
	{
        S.push_fmt(L"(%d) %s", code, buffer);
		::LocalFree(buffer);
	}
    else {
        S.push(L"(winapi error: FormatMessage)");
    }

    return 1;
}

BEG_LIB_API()
    LIBAPI2("errno", errno_)
    LIBAPI(error)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{
    S.copy(-1);
    S.setglobal("winapi");
    S.setfield("_errno", 0);
}

//////////////////////////////////////////////////////////////////////////

END_LIB_NAMESPACE()
