#include <taolua/taolua.h>
#include "console.h"

namespace taolua {
namespace console {

DECL_OBJECT(ConsoleObject)
{
protected:
    struct ConsoleEvent
    {
        enum Value
        {
            Focus,
            Mouse,
            Keyboard,
            Menu,
            Size,
        };
    };

    struct LuaInputRecord
    {
        const INPUT_RECORD& ir;
        LuaInputRecord(const INPUT_RECORD& ir_) : ir(ir_) {}
        void to_table(LuaWrapper& G) const
        {
            switch(ir.EventType)
            {
            case MOUSE_EVENT:
            {
                const auto& me = ir.Event.MouseEvent;
                G.newtable(0, 5);
                G.setfield("x",         me.dwMousePosition.X);
                G.setfield("y",         me.dwMousePosition.Y);
                G.setfield("button",    me.dwButtonState);
                G.setfield("control",   me.dwControlKeyState);
                G.setfield("flags",     me.dwEventFlags);
                break;
            }
            case KEY_EVENT:
            {
                const auto& ke = ir.Event.KeyEvent;
                G.newtable(0, 6);
                G.setfield("pressed",   BoolVal(ke.bKeyDown));
                G.setfield("count",     ke.wRepeatCount);
                G.setfield("keycode",   ke.wVirtualKeyCode);
                G.setfield("scancode",  ke.wVirtualScanCode);
                G.setfield("char",      std::wstring(1, ke.uChar.UnicodeChar));
                G.setfield("control",   ke.dwControlKeyState);
                break;
            }
            }
        }
    };

public:
    ConsoleObject() 
    {
        _hwnd = ::GetConsoleWindow();
        _stdin = ::GetStdHandle(STD_INPUT_HANDLE);
        _stdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
        _stderr = ::GetStdHandle(STD_ERROR_HANDLE);
        _last_mode = 0;
    }

    BEG_OBJ_API_GC(console, ConsoleObject)
        OBJAPI(title)
        OBJAPI(hwnd)
        OBJAPI2("stdin", stdin_)
        OBJAPI2("stdout", stdout_)
        OBJAPI2("stderr", stderr_)
        OBJAPI(fullscreen)
        OBJAPI(windowed)
        OBJAPI(interactive)
    END_OBJ_API()

    LUAAPI(title)
    {
        DECL_THIS;
        auto buf = std::make_unique<wchar_t[]>(64 * 1024);
        if(auto len = ::GetConsoleTitle(buf.get(), 64 * 1024)) {
            G.push(buf.get(), len);
        }
        else {
            G.push(L"");
        }
        return 1;
    }

    LUAAPI(hwnd)
    {
        DECL_THIS;
        G.push_as_int(O._hwnd);
        return 1;
    }

    LUAAPI(stdin_)
    {
        DECL_THIS;
        G.push_as_int(O._stdin);
        return 1;
    }

    LUAAPI(stdout_)
    {
        DECL_THIS;
        G.push_as_int(O._stdout);
        return 1;
    }

    LUAAPI(stderr_)
    {
        DECL_THIS;
        G.push_as_int(O._stderr);
        return 1;
    }

    static int _window_helper(LuaWrapper& G, HANDLE handle, bool fullscreen)
    {
        COORD co;
        BoolVal bv = ::SetConsoleDisplayMode(handle,
            fullscreen ? CONSOLE_FULLSCREEN_MODE : CONSOLE_WINDOWED_MODE,
            &co);

        if(bv) {
            G.push(bv);
            G.push(co.X);
            G.push(co.Y);
            return 3;
        }
        else {
            G.push(bv);
            return 1;
        }
    }

    LUAAPI(fullscreen)
    {
        DECL_THIS;
        return _window_helper(G, O._stdout, true);
    }

    LUAAPI(windowed)
    {
        DECL_THIS;
        return _window_helper(G, O._stdout, false);
    }

    LUAAPI(interactive)
    {
        DECL_THIS;
        BoolVal enter = G.check_bool(2);
        // 禁止不配对的调用
        if(enter && O._last_mode)
            G.argerr(2, "can only enter once");

        // allow multiple leave (but returns false)
        if(!enter && !O._last_mode) {
            G.push(false);
            return 1;
        }

        if(enter) {
            BoolVal eax;
            G.check_func(3);
            if(::GetConsoleMode(O._stdin, &O._last_mode)) {
                auto new_mode = O._last_mode;
                new_mode &= ~ENABLE_QUICK_EDIT_MODE;                    // 快速编辑模式下收不到部分事件
                new_mode |= ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

                if(::SetConsoleMode(O._stdin, new_mode)) {
                    auto la_process_event = [&](const INPUT_RECORD& ir)
                    {
                        G.copy(3); // callback

                        int top = G.gettop();

                        switch(ir.EventType)
                        {
                        case FOCUS_EVENT:
                        {
                            const auto& fe = ir.Event.FocusEvent;
                            G.push(ConsoleEvent::Focus);
                            G.push(BoolVal(fe.bSetFocus));
                            break;
                        }
                        case MENU_EVENT:
                        {
                            const auto& me = ir.Event.MenuEvent;
                            G.push(ConsoleEvent::Menu);
                            G.push(me.dwCommandId);
                            break;
                        }
                        case WINDOW_BUFFER_SIZE_EVENT:
                        {
                            const auto& se = ir.Event.WindowBufferSizeEvent;
                            G.push(ConsoleEvent::Size);
                            G.push(se.dwSize.X);
                            G.push(se.dwSize.Y);
                            break;
                        }
                        case MOUSE_EVENT:
                        case KEY_EVENT:
                        {
                            if(ir.EventType == MOUSE_EVENT)     G.push(ConsoleEvent::Mouse);
                            else if(ir.EventType == KEY_EVENT)  G.push(ConsoleEvent::Keyboard);
                            G.push(LuaInputRecord(ir));
                            break;
                        }
                        }

                        G.call(G.gettop() - top, 1);
                        BoolVal again = G.opt_bool(-1, true);
                        G.pop();
                        return again;
                    };

                    for(;;) {
                        BoolVal again;
                        DWORD n;
                        constexpr size_t irc = 1;
                        INPUT_RECORD irs[irc];
                        if(::ReadConsoleInput(O._stdin, irs, irc, &n)) {
                            for(size_t i = 0; i < n; ++i) {
                                again = la_process_event(irs[i]);
                                if(!again) break;
                            }
                        }

                        if(!again) break;
                    }

                    eax = true;
                }
            }

            if(!eax) {
                SAVE_LAST_ERROR;
            }

            G.push(eax);
            return 1;
        }
        // leave
        else {
            ::SetConsoleMode(O._stdin, O._last_mode);
            O._last_mode = 0;
            G.push(true);
            return 1;
        }
    }

protected:
    HWND   _hwnd;
    HANDLE _stdin;
    HANDLE _stdout;
    HANDLE _stderr;
    DWORD  _last_mode;
};

DECL_MODULE_MAGIC(__init__)
{

}

LUAAPI(this_)
{
    DECL_WRAP;
    G.push<ConsoleObject>();
    return 1;
}

LUAAPI(exec)
{
    DECL_WRAP;
    auto cmd = G.check_str(1);
    int ret = ::_wsystem(cmd.c_str());
    G.push(ret);
    return 1;
}

BEG_LIB_API()
    LIBAPI2("this", this_)
    LIBAPI(exec)
END_LIB_API()

} }
