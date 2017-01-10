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
        DECL_THIS();
        S.push_fmt(L"%s { PID: %d }", __namew__(), O._pid);
        return 1;
    }

    LUAAPI(threads)
    {
        DECL_THIS();
        bool succ = false;
        AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0 /* O._pid ignored */);
        if(hSnapshot) {
            THREADENTRY32 te = {sizeof(te)};
            if(::Thread32First(hSnapshot, &te)) {
                S.newtable();
                do {
                    if(te.th32OwnerProcessID == O._pid) {
                        S.push(te.th32ThreadID);
                        S.newtable();
                        S.setfield("pid", te.th32OwnerProcessID);
                        S.rawset(-3);
                    }
                } while(::Thread32Next(hSnapshot, &te));
                succ = true;
            }
        }

        return succ ? 1 : 0;
    }

    LUAAPI(modules)
    {
        DECL_THIS();
        bool succ = false;
        AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, O._pid);
        if(hSnapshot) {
            MODULEENTRY32 me = {sizeof(me)};
            if(::Module32First(hSnapshot, &me)) {
                int i = 1;
                S.newtable();
                do {
                    S.newtable();
                    S.setfield("pid",   me.th32ProcessID);
                    S.setfield("base",  (lua_Integer)me.modBaseAddr);
                    S.setfield("size",  me.modBaseSize);
                    S.setfield("handle", (lua_Integer)me.hModule);
                    S.setfield("name",  me.szModule);
                    S.setfield("path",  me.szExePath);
                    S.rawseti(-2, i++);
                } while(::Module32Next(hSnapshot, &me));
                succ = true;
            }
        }

        return succ ? 1 : 0;
    }

    LUAAPI(term)
    {
        DECL_THIS();
        AutoHandle handle = ::OpenProcess(PROCESS_TERMINATE, FALSE, O._pid);
        if(handle) {
            auto code = S.opt_integer<DWORD>(2, -1);
            BoolVal ok = ::TerminateProcess(handle, code);
            S.push(ok);
        }
        else {
            SAVE_LAST_ERROR;
            S.push(false);
        }
        return 1;
    }

protected:
    DWORD _pid;
};

//////////////////////////////////////////////////////////////////////////

LUAAPI(topsobj)
{
    DECL_WRAP();
    auto pid = S.check_int(1);
    S.push<ProcessObject>(pid);
    return 1;
}

LUAAPI(processes)
{
    DECL_WRAP();
    bool succ = false;
    bool has_special = S.opt_bool(1, false);
    AutoHandle hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hSnapshot) {
        PROCESSENTRY32 pe = {sizeof(pe)};
        if(::Process32First(hSnapshot, &pe)) {
            S.newtable();
            do {
                if(!has_special && pe.th32ProcessID == 0)
                    continue;

                S.push(pe.th32ProcessID);
                S.newtable(0, 4);
                S.setfield("pid",           pe.th32ProcessID);
                S.setfield("ppid",          pe.th32ParentProcessID);
                S.setfield("name",          pe.szExeFile);
                S.setfield("thread_count",  pe.cntThreads);
                S.rawset(-3);
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
