local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local wsh = taolua.windows_shell
print(wsh.create_shortcut([[C:\Users\Tao\Desktop\a.lnk]], [[C:\Users\Tao\Desktop\taolua\taolua.sln]]))

