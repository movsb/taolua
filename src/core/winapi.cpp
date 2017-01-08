#include "stdafx.h"
#include "winapi.h"

BEG_LIB_NAMESPACE(winapi)

LUAAPI(errno_)
{
    DECL_WRAP;
    G.push(::GetLastError());
    if(G.isinteger(1))
        ::SetLastError((DWORD)G.tointeger(1));
    return 1;
}

LUAAPI(error)
{
    DECL_WRAP;
    wchar_t* buffer;
    DWORD code = G.opt_int(1, GetLastError());

	if (::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR)&buffer, 1, NULL))
	{
        G.push(buffer);
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
