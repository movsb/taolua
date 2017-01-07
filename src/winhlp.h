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

}
