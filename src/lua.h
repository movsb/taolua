#pragma once

#include "charset.h"

namespace taolua {

#define MODULE(name) lua.newlib(#name, taolua::name::methods)

#define DECL_METHODS extern const luaL_Reg methods[]
#define DECL_WRAP LuaWrapper G(L)
#define LIBAPI(name) static int name(lua_State* L)

#define BEG_LUA_API() \
    const luaL_Reg methods[]\
    {
#define LUAAPI(name) {#name, name},
#define END_LUA_API()   {nullptr, nullptr} };

#define BEG_OBJ_API(T, name_) \
    static T* check_this(lua_State* L) {return reinterpret_cast<T*>(luaL_checkudata(L, 1, name()));}\
    static const char* const name() { return #name_; }\
    static const luaL_Reg* const apis()\
    {\
        static luaL_Reg _apis[] = {
#define END_OBJ_API() \
            {nullptr, nullptr}\
        };\
        return _apis;\
    }
#define OBJAPI LUAAPI
#define DECL_THIS auto& G = *check_this(L)

class LuaWrapper
{
public:
    LuaWrapper(lua_State* L) { _L = L; }

protected:
    friend class TaoLua;
    LuaWrapper() {}

public:
    void newtable(int na = 0, int nk = 0)                   { return lua_createtable(_L, na, nk); }
    void newtable(const std::vector<std::wstring>& arr);
    int  newmetatable(const char* name)                     { return luaL_newmetatable(_L, name); }
    void* newud(size_t size)                                { return lua_newuserdata(_L, size); }

    void setglobal(const char* name)                        { return lua_setglobal(_L, name); }
    int  getglobal(const char* name)                        { return lua_getglobal(_L, name); }

    void setmetatable(int i)                                { return (void)lua_setmetatable(_L, i); }
    int  getmetatable(const char* name)                     { return luaL_getmetatable(_L, name); }

    void setfuncs(const luaL_Reg fns[], int nup = 0)        { return luaL_setfuncs(_L, fns, nup); }
    void setfield(int index, const char* key)               { return lua_setfield(_L, index, key); }

    void rawset(int i)                                      { return lua_rawset(_L, i); }
    void rawseti(int t, int i)                              { return lua_rawseti(_L, t, i); }

    void copy(int i)                                        { return lua_pushvalue(_L, i); }
    void push()                                             { return lua_pushnil(_L); }
    void push(int i)                                        { return push(lua_Integer(i)); }
    void push(lua_Integer i)                                { return lua_pushinteger(_L, i); }
    void push(lua_Number n)                                 { return lua_pushnumber(_L, n); }
    void push(const char* s)                                { return (void)lua_pushstring(_L, s); }
    void push(const std::wstring& s)                        { auto _s(to_utf8(s)); return (void)lua_pushlstring(_L, _s.c_str(), _s.size()); }
    void push(bool b)                                       { return lua_pushboolean(_L, b); }
    void push(lua_CFunction f, int nup = 0)                 { return lua_pushcclosure(_L, f, nup); }
    void push(void* p)                                      { return lua_pushlightuserdata(_L, p); }
    template<typename T, typename... Args>
        T* push(Args ...args)
        {
            auto p = new (newud(sizeof(T))) T(std::forward<Args>(args)...);

            if(getmetatable(T::name()) == LUA_TNIL) {
                pop();
                newmetatable(T::name());
                setfuncs(T::apis(), 0);
                copy(-1);
                setfield(-2, "__index");
            }

            setmetatable(-2);

            return p;
        }

    void pop(int n = 1)                                     { return lua_pop(_L, n); }

    void call(int na= 0, int nr = 0)                        { return lua_call(_L, na, nr); }
    int pcall(int na= 0, int nr = 0, int eh = 0)            { return lua_pcall(_L, na, nr, eh); }

    std::string     chk_raw_str(int i)                      { const char* s; size_t n; s = luaL_checklstring(_L, i, &n); return {s,n}; }
    std::wstring    chk_str(int i)                          { return from_utf8(chk_raw_str(i)); }
    static bool     _lua_checkbool(lua_State* L, int i)     { luaL_argcheck(L, lua_isboolean(L, i), i, "bool expected"); return !!lua_toboolean(L, i); }
    bool            chk_bool(int i)                         { return _lua_checkbool(_L, i); }
    bool            opt_bool(int i, bool def)               { return luaL_opt(_L, _lua_checkbool, i, def); }

protected:
    lua_State* _L;
};

class TaoLua : public LuaWrapper
{
public:
    TaoLua() {}
    operator lua_State*() { return _L; }

public:
    // state
    void open();
    void close();
    void main();

    // lib
    void newlib(const char* name, const luaL_Reg fns[]);

    // load
    int exec(const std::wstring& file);

protected:
    void _init_global();
};

}
