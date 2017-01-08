#pragma once

namespace taolua
{

template<typename T, typename BOOLER, typename CLOSER>
class AutoCloseT
{
public:
    AutoCloseT(T t) : value(t) {}
    ~AutoCloseT() { CLOSER()(value); }
    operator bool() const { return BOOLER()(value); }
    operator T() const { return value; }

protected:
    T value;
};

struct HandleBooler { bool operator()(HANDLE h) const { return h != nullptr && h != INVALID_HANDLE_VALUE; } };
struct HandleCloser { bool operator()(HANDLE h) const { return !!::CloseHandle(h); } };
typedef AutoCloseT<HANDLE, HandleBooler, HandleCloser> AutoHandle;

class BoolVal
{
public:
    BoolVal(BOOL b) : _b(b) {}
    BoolVal(bool b) : _b(b) {}

    operator BOOL() const { return _b; }
    operator bool() const { return !!_b; }

    bool to_lua() const { return operator bool(); }

protected:
    BOOL _b;
};

#define SAVE_LAST_ERROR \
    if(G.getglobal("winapi") == LUA_TTABLE) {\
        G.setfield("_errno", ::GetLastError());\
        G.pop();\
    }
}
