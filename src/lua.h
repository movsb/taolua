#pragma once

#include "charset.h"

namespace taolua {

#define MODULE(name) lua.newlib(#name, taolua::name::__methods__, taolua::name::__init__)

#define DECL_MODULE_MAGIC(name) void name(LuaWrapper G)
#define DECL_MODULE extern const luaL_Reg __methods__[]; DECL_MODULE_MAGIC(__init__)
#define DECL_WRAP LuaWrapper G(L)
#define LUAAPI(name) static int name(lua_State* L)
#define STRINTPAIR(name)            {#name, name},
#define STRINTPAIR2(name1,name2)    {name1, name2},

#define BEG_LIB_API() \
    const luaL_Reg __methods__[]\
    {
#define LIBAPI      STRINTPAIR
#define LIBAPI2     STRINTPAIR2
#define END_LIB_API()   {nullptr, nullptr} };

#define DECL_OBJECT(T) class T
#define BEG_OBJ_API(T, name_) \
    static T* check_this(lua_State* L) {return reinterpret_cast<T*>(luaL_checkudata(L, 1, __name__()));}\
    static const char* const __name__() { return "" ## name_; }\
    static const wchar_t* const __namew__() { return L"" ## name_; }\
    static const luaL_Reg* const __apis__()\
    {\
        static luaL_Reg s_apis[] = {
#define END_OBJ_API() \
            {nullptr, nullptr}\
        };\
        return s_apis;\
    }
#define OBJAPI      STRINTPAIR
#define OBJAPI2     STRINTPAIR2
#define DECL_THIS LuaWrapper G(L); auto& O = *check_this(L)

#define BEG_LIB_NAMESPACE(name) namespace taolua { namespace name {
#define END_LIB_NAMESPACE() }}

template<typename T, typename B>
struct OptStrT
{
    OptStrT(B const * s, size_t n, bool b, B const * d) : _s(s ? s : T()), _b(b), _d(d) { }
    OptStrT(T&& s, bool b, B const* d) : _s(s), _b(b), _d(d) { }
    T* operator->() { return &_s; }
    operator T&() { return _s; }
    operator B const *() const { return *this ? _s.c_str() : _d; }
    operator bool() const { return _b; }
    bool        _b;
    T           _s;
    B const *   _d;
};

typedef OptStrT<std::string, char>      OptStrRaw;
typedef OptStrT<std::wstring, wchar_t>  OptStr;

typedef std::unordered_map<const char*, int> CStr2Int;

class LuaWrapper
{
public:
    LuaWrapper(lua_State* L) { _L = L; }

protected:
    friend class TaoLua;
    LuaWrapper() {}

public:
    // basic stack manipulation
    int     gettop()                                        { return lua_gettop(_L); }
    void    settop(int i)                                   { return lua_settop(_L, i); }
    void    copy(int i)                                     { return lua_pushvalue(_L, i); }
    void    copy(int from, int to)                          { return lua_copy(_L, from, to); }
    void    pop(int n = 1)                                  { return lua_pop(_L, n); }
    void    remove(int i)                                   { return lua_remove(_L, i); }

    // accessors (stack -> C)
    int     type(int i)                                     { return lua_type(_L, i); }
    const char* type_name(int i)                            { return luaL_typename(_L, i); }

    bool    isnil(int i)                                    { return !!lua_isnil(_L, i); }
    bool    isnone(int i)                                   { return !!lua_isnone(_L, i); }
    bool    isnoneornil(int i)                              { return !!lua_isnoneornil(_L, i); }
    bool    isbool(int i)                                   { return !!lua_isboolean(_L, i); }
    bool    isnumber(int i)                                 { return !!lua_isnumber(_L, i); }
    bool    isstring(int i)                                 { return !!lua_isstring(_L, i); }
    bool    iscfunc(int i)                                  { return !!lua_iscfunction(_L, i); }
    bool    isfunc(int i)                                   { return !!lua_iscfunction(_L, i); }
    bool    isinteger(int i)                                { return !!lua_isinteger(_L, i); }
    bool    isuserdata(int i)                               { return !!lua_isuserdata(_L, i); }
    bool    islightuserdata(int i)                          { return !!lua_islightuserdata(_L, i); }
    bool    istable(int i)                                  { return !!lua_istable(_L, i); }

    lua_Integer     tointeger(int i)                        { return lua_tointeger(_L, i); }
    lua_Number      tonumber(int i)                         { return lua_tonumber(_L, i); }
    bool            tobool(int i)                           { return !!lua_toboolean(_L, i); }
    const char*     tostring(int i)                         { return lua_tostring(_L, i); }
    const char*     tolstring(int i, size_t* len)           { return lua_tolstring(_L, i, len); }
    lua_CFunction   tocfunc(int i)                          { return lua_tocfunction(_L, i); }
    void*           touserdata(int i)                       { return lua_touserdata(_L, i); }

    std::string     check_str_raw(int i)                    { const char* s; size_t n; s = luaL_checklstring(_L, i, &n); return {s,n}; }
    std::wstring    check_str(int i)                        { return from_utf8(check_str_raw(i)); }
    OptStrRaw       opt_str_raw(int i, const char* def="")  { const char* s; size_t n; s = luaL_optlstring(_L, i, def, &n); return {s, n, s != def, def}; }
    OptStr          opt_str(int i, const wchar_t* def=L"")  { auto raw = opt_str_raw(i, def ? to_utf8(def).c_str() : nullptr); return { from_utf8(raw), raw, def};}

    lua_Number      check_number(int i)                     { return luaL_checknumber(_L, i); }
    lua_Number      opt_number(int i, lua_Number def)       { return luaL_optnumber(_L, i, def); }

    lua_Integer     check_integer(int i)                    { return luaL_checkinteger(_L, i);}
    lua_Integer     opt_integer(int i, lua_Integer def)     { return luaL_optinteger(_L, i, def);}
    template<typename T>
    T               check_integer(int i)                    { return (T)check_int(i); }
    template<typename T>
    T               opt_integer(int i, T def)               { return (T)opt_int(i, (lua_Integer)def); }

    int             check_int(int i)                        { return (int)luaL_checkinteger(_L, i);}
    int             opt_int(int i, int def)                 { return (int)luaL_optinteger(_L, i, def);}

    static bool     _lua_checkbool(lua_State* L, int i)     { luaL_argcheck(L, lua_isboolean(L, i), i, "bool expected"); return !!lua_toboolean(L, i); }
    bool            check_bool(int i)                       { return _lua_checkbool(_L, i); }
    bool            opt_bool(int i, bool def)               { return luaL_opt(_L, _lua_checkbool, i, def); }

    template<typename T>
    T               check_udata(int i)                      { if(islightuserdata(i)) return (T)touserdata(i); argerr(i, (typeid(T).name() + std::string(" expected")).c_str()); return T(); }
    template<typename T>
    T&              check_object(int i)                     { return *(T*)luaL_checkudata(_L, i, T::__name__()); }

    // push functions (C -> stack)
    void push()                                             { return lua_pushnil(_L); }
    void push(int i)                                        { return push(lua_Integer(i)); }
    void push(lua_Integer i)                                { return lua_pushinteger(_L, i); }
    void push(lua_Number n)                                 { return lua_pushnumber(_L, n); }
    void push(const char* s)                                { return (void)lua_pushstring(_L, s); }
    void push(const char* s, size_t n)                      { return (void)lua_pushlstring(_L, s, n); }
    void push(const wchar_t* s)                             { return push(to_utf8(s)); }
    void push(const wchar_t* s, size_t n, bool raw = false) { return raw ? push((const char*)s, n * 2) : push(to_utf8({s, n})); }
    // const char* push(const char* f, ...)                    { va_list va; va_start(va, f); const char* p = lua_pushvfstring(_L, f, va); va_end(va); return p; }
    void push_fmt(const wchar_t* f, ...)
    {
        va_list va;
        wchar_t buf[1024];
        wchar_t* p;
        va_start(va, f);
        size_t n = _vscwprintf(f, va);
        p = n > _countof(buf) - 1 ? new wchar_t[n + 1] : buf;
        _vswprintf(p, f, va);
        push(p, n);
        if(p != buf) delete[] p;
    }
    void push(const std::wstring& s, bool raw = false)      { return raw ? push(s.c_str(), s.size(), true) : push(to_utf8(s)); }
    void push(const std::string& s)                         { return (void)push(s.c_str(), s.size()); }
    void push(bool b)                                       { return lua_pushboolean(_L, b); }
    void push(lua_CFunction f, int nup = 0)                 { return lua_pushcclosure(_L, f, nup); }
    void push(void* p)                                      { return lua_pushlightuserdata(_L, p); }
    template<typename T, typename... Args>
        T* push(Args ...args)
        {
            auto p = new (newud(sizeof(T))) T(std::forward<Args>(args)...);

            if(getmetatable(T::__name__()) == LUA_TNIL) {
                pop();
                newmetatable(T::__name__());
                setfuncs(T::__apis__(), 0);
                copy(-1);
                setfield(-2, "__index");
            }

            setmetatable(-2);

            return p;
        }

    template<typename T, typename = decltype(&T::to_lua)>
    void push(const T& v) { push(v.to_lua()); }

    void push(DWORD dw)                                     { return push(lua_Integer(dw)); }

    // get functions (Lua -> stack)
    int     getglobal(const char* name)                     { return lua_getglobal(_L, name); }
    int     gettable(int t)                                 { return lua_gettable(_L, t); }
    int     getfield(int t, const char* k)                  { return lua_getfield(_L, t, k); }
    int     getfield(const char* k, int t = -1)             { return lua_getfield(_L, t, k); }
    int     geti(int t, lua_Integer n)                      { return lua_geti(_L, t, n); }
    int     rawget(int t)                                   { return lua_rawget(_L, t); }
    int     rawgeti(int t, lua_Integer n)                   { return lua_rawgeti(_L, t, n); }
    int     rawgetp(int t, const void* p)                   { return lua_rawgetp(_L, t, p); }

    void    newtable(int na = 0, int nk = 0)                { return lua_createtable(_L, na, nk); }
    void    newtable(const std::vector<std::wstring>& arr);
    void    newtable(const CStr2Int& map);
    int     newmetatable(const char* name)                  { return luaL_newmetatable(_L, name); }
    void*   newud(size_t size)                              { return lua_newuserdata(_L, size); }
    int     getmetatable(int o)                             { return lua_getmetatable(_L, o); }
    int     getmetatable(const char* name)                  { return luaL_getmetatable(_L, name); }

    // set functions (stack -> Lua)
    void    setglobal(const char* name)                     { return lua_setglobal(_L, name); }
    void    settable(int t)                                 { return lua_settable(_L, t); }
    void    setfield(int t, const char* k)                  { return lua_setfield(_L, t, k); }
    template<typename T>
        void    setfield(const char* k, const T& t)         { push(t); return setfield(-2, k); }
    void    seti(int t, lua_Integer n)                      { return lua_seti(_L, t, n); }
    void    rawset(int i)                                   { return lua_rawset(_L, i); }
    void    rawseti(int t, lua_Integer n)                   { return lua_rawseti(_L, t, n); }
    void    rawsetp(int t, const void* p)                   { return lua_rawsetp(_L, t, p); }
    void    setmetatable(int i)                             { return (void)lua_setmetatable(_L, i); }

    // load and call
    void call(int na= 0, int nr = 0)                        { return lua_call(_L, na, nr); }
    int pcall(int na= 0, int nr = 0, int eh = 0)            { return lua_pcall(_L, na, nr, eh); }

    // misc
    int     argerr(int i, const char* msg)                  { return luaL_argerror(_L, i, msg); }
    int     next(int t)                                     { return lua_next(_L, t); }
    void    setfuncs(const luaL_Reg fns[], int nup = 0)     { return luaL_setfuncs(_L, fns, nup); }

    // refs
    int     ref(int t = LUA_REGISTRYINDEX)                  { return luaL_ref(_L, t);}
    void    unref(int ref, int t = LUA_REGISTRYINDEX)       { return luaL_unref(_L, t, ref);}


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

    void newlib(const char* name, const luaL_Reg fns[], void(*init)(LuaWrapper G));

    // load
    int exec(const std::wstring& file);

protected:
    void _init_global();
};

}
