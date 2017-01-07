#include "stdafx.h"
#include "psapi.h"

#include <TlHelp32.h>

BEG_LIB_NAMESPACE(psapi)

LUAAPI(list_processes)
{
    DECL_WRAP;
    AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot) {
        PROCESSENTRY32 pe = {sizeof(pe)};
        if(::Process32First(hSnapshot, &pe)) {
            G.push(true);
            G.newtable();
            do {
                G.push(pe.th32ProcessID);
                G.newtable();
                G.setfield("pid",           pe.th32ProcessID);
                G.setfield("ppid",          pe.th32ParentProcessID);
                G.setfield("name",          pe.szExeFile);
                G.setfield("thread_count",  pe.cntThreads);
                G.rawset(-3);
            } while(::Process32Next(hSnapshot, &pe));
            return 2;
        }
    }

    G.push(false);
    return 1;
}


BEG_LIB_API()
    LIBAPI(list_processes)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

//////////////////////////////////////////////////////////////////////////


END_LIB_NAMESPACE()
