#include "stdafx.h"
#include "psapi.h"

#include <TlHelp32.h>

BEG_LIB_NAMESPACE(psapi)

DECL_OBJECT(ProcessObject)
{
public:
    ProcessObject(DWORD pid)
        : _pid(pid) { }

    BEG_OBJ_API(ProcessObject, "psapi::ProcessObject")
        OBJAPI(__tostring)
        OBJAPI(threads)
        OBJAPI(modules)
        OBJAPI(term)
    END_OBJ_API()

public:
    LUAAPI(__tostring)
    {
        DECL_THIS;
        G.push_fmt(L"%s { PID: %d }", __namew__(), O._pid);
        return 1;
    }

    LUAAPI(threads)
    {
        DECL_THIS;
        bool succ = false;
        AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0 /* O._pid ignored */);
        if(hSnapshot) {
            THREADENTRY32 te = {sizeof(te)};
            if(::Thread32First(hSnapshot, &te)) {
                G.newtable();
                do {
                    if(te.th32OwnerProcessID == O._pid) {
                        G.push(te.th32ThreadID);
                        G.newtable();
                        G.setfield("pid", te.th32OwnerProcessID);
                        G.rawset(-3);
                    }
                } while(::Thread32Next(hSnapshot, &te));
                succ = true;
            }
        }

        return succ ? 1 : 0;
    }

    LUAAPI(modules)
    {
        DECL_THIS;
        bool succ = false;
        AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, O._pid);
        if(hSnapshot) {
            MODULEENTRY32 me = {sizeof(me)};
            if(::Module32First(hSnapshot, &me)) {
                int i = 1;
                G.newtable();
                do {
                    G.newtable();
                    G.setfield("pid",   me.th32ProcessID);
                    G.setfield("base",  (lua_Integer)me.modBaseAddr);
                    G.setfield("size",  me.modBaseSize);
                    G.setfield("handle", (lua_Integer)me.hModule);
                    G.setfield("name",  me.szModule);
                    G.setfield("path",  me.szExePath);
                    G.rawseti(-2, i++);
                } while(::Module32Next(hSnapshot, &me));
                succ = true;
            }
        }

        return succ ? 1 : 0;
    }

    LUAAPI(term)
    {
        DECL_THIS;
        AutoHandle handle = ::OpenProcess(PROCESS_TERMINATE, FALSE, O._pid);
        if(handle) {
            auto code = G.opt_integer<DWORD>(2, -1);
            BoolVal ok = ::TerminateProcess(handle, code);
            G.push(ok);
        }
        else {
            SAVE_LAST_ERROR;
            G.push(false);
        }
        return 1;
    }

protected:
    DWORD _pid;
};

//////////////////////////////////////////////////////////////////////////

LUAAPI(topsobj)
{
    DECL_WRAP;
    auto pid = G.check_int(1);
    G.push<ProcessObject>(pid);
    return 1;
}

LUAAPI(processes)
{
    DECL_WRAP;
    bool succ = false;
    bool has_special = G.opt_bool(1, false);
    AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot) {
        PROCESSENTRY32 pe = {sizeof(pe)};
        if(::Process32First(hSnapshot, &pe)) {
            G.newtable();
            do {
                if(!has_special && pe.th32ProcessID == 0)
                    continue;

                G.push(pe.th32ProcessID);
                G.newtable(0, 4);
                G.setfield("pid",           pe.th32ProcessID);
                G.setfield("ppid",          pe.th32ParentProcessID);
                G.setfield("name",          pe.szExeFile);
                G.setfield("thread_count",  pe.cntThreads);
                G.rawset(-3);
            } while(::Process32Next(hSnapshot, &pe));
            succ = true;
        }
    }

    return succ ? 1 : 0;
}

BEG_LIB_API()
    LIBAPI(processes)
    LIBAPI(topsobj)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

//////////////////////////////////////////////////////////////////////////


END_LIB_NAMESPACE()
