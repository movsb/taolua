local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local dialog = taolua.dialog

print(dialog.choose_color(0xff,0x00,0xff))
print(dialog.get_open_file())

