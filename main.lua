local wm = taolua.window_manage

local wnd = wm.get_foreground_window()
if wnd then
    local wo = wm.towinobj(wnd)
    print(wo)
    print(wo.title)
    print(wo:class())
    print(wo:title())
end

