#include "stdafx.h"

#include "lua.h"
#include "taolua.h"

namespace taolua
{

void TaoLua::open()
{
    _L = luaL_newstate();
    assert(_L);
    luaL_openlibs(_L);

    _init_global();
}

void TaoLua::close()
{
    lua_close(_L);
    _L = nullptr;
}

void TaoLua::main()
{
    getglobal("taolua");
}

void TaoLua::newlib(const char* name, const luaL_Reg fns[], void(*init)(LuaWrapper G))
{
    main();
    newtable();
    setfuncs(fns);
    init(_L);
    setfield(-2, name);
    pop();
}

bool TaoLua::load(TaoLua* __this , const std::wstring& lib)
{
    // -1: invalid, 0: file, 1: folder
    auto get_file_type = [](const std::wstring& file) {
        DWORD a = ::GetFileAttributes(file.c_str());
        return a == INVALID_FILE_ATTRIBUTES ? -1 : !(a & FILE_ATTRIBUTE_DIRECTORY) ? 0 : 1;
    };

    auto get_root = [] {
        wchar_t path[MAX_PATH];
        path[0] = L'\0';
        ::GetModuleFileName(nullptr, path, _countof(path));
        *wcsrchr(path, L'\\') = L'\0';
        return std::wstring(path);
    };

    auto get_target = [&] {
        static std::wstring paths[] = {
            get_root() + L'/' + lib + L".lua",
            get_root() + L'/' + lib + L".dll",
            get_root() + L'/' + lib + L"/main.lua",
            get_root() + L'/' + lib + L"/main.dll",
        };

        for(size_t i = 0; i < _countof(paths); i++) {
            if(get_file_type(paths[i]) == 0)
                return paths[i];
        }
        
        return std::wstring();
    };

    // -1: bad, 0: lua, 1: dll
    auto get_init_type = [&](const std::wstring& f) {
        if(get_file_type(f) == -1)
            return -1;
        else if(std::regex_match(f, std::wregex(LR"(.*\.lua)", std::regex_constants::icase)))
            return 0;
        else if(std::regex_match(f, std::wregex(LR"(.*\.dll)", std::regex_constants::icase)))
            return 1;
        return -1;
    };

    auto target = get_target();
    auto init_type = get_init_type(target);
    if(init_type == -1) {
        __this->push_fmt(L"module init file not found for: %s", lib.c_str());
        return false;
    }
    else if(init_type == 0) {
        return __this->exec(target) == LUA_OK;
    }
    else if(init_type == 1) {
        HMODULE hMod = ::LoadLibrary(target.c_str());
        if(!hMod) {
            __this->push_fmt(L"failed to LoadLibrary: %s(%d)", target.c_str(), ::GetLastError());
            return false;
        }

        using TaoLuaInit = bool (*)(TaoLuaInitParams* args);
        auto _init = (TaoLuaInit)::GetProcAddress(hMod, "taolua_init");
        if(!_init) {
            __this->push_fmt(L"invalid module entry for: %s", target.c_str());
            ::FreeLibrary(hMod);
            return false;
        }

        static TaoLua* __G;
        auto la_newlib = [](const char* name, const luaL_Reg fns[], void(*init)(LuaWrapper S)) {
            __G->newlib(name, fns, init);
        };

        TaoLuaInitParams args;
        args.newlib = la_newlib;
        __G = this;
        BoolVal br = _init(&args);
        if(!br) __this->push_fmt(L"failed to call init func: %s", target.c_str());
        return br;
    }

    __this->push(L"unknown error.");
    return false;
}

int TaoLua::lua_load(lua_State* L)
{
    DECL_WRAP;
    G.getglobal("taolua");
    G.getfield("__this__");
    auto __this = G.check_udata<TaoLua*>(-1);
    G.pop();
    auto lib = G.check_str(1);
    auto r = __this->load(__this, lib);
    if(!r) {
        G.error();
    }
    return 0;
}

int TaoLua::exec(const std::wstring& file)
{
    std::ifstream f(file, std::ios::in | std::ios::binary);

    if(!f.is_open()) {
        printf("file not found: %s\n", to_utf8(file).c_str());
        return -1;
    }

    f.seekg(0, f.end);
    auto size = (size_t)f.tellg();
    auto buf = std::make_unique<char[]>(size + 1);
    f.seekg(f.beg);
    f.read(buf.get(), size);
    buf.get()[size] = '\0';

    int r = luaL_dostring(_L, buf.get());
    if(r != LUA_OK) {
        auto err = to_utf8(check_str(-1));
        printf("error executing lua: %s\n", err.c_str());
        pop();
    }

    return r;
}

int TaoLua::lua_exec(lua_State* L)
{
    DECL_WRAP;
    G.getglobal("taolua");
    G.getfield("__this__");
    auto __this = G.check_udata<TaoLua*>(-1);
    G.pop();
    auto file = G.check_str(1);
    auto r = __this->exec(file);
    G.push(r);
    return 1;
}

void TaoLua::_init_global()
{
    newtable();

    setfield("__this__", this);
    setfield("exec", lua_exec);
    setfield("load", lua_load);

    setglobal("taolua");

    auto sleep = [](lua_State* L) -> int {
        ::Sleep((DWORD)luaL_checkinteger(L, 1));
        return 0;
    };

    push(sleep);
    setglobal("sleep");
}

}
