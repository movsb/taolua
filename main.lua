local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local fs = taolua.file_system

print(fs.ln([[a.exe]], [[C:\Users\tao\Desktop\Thunder9.1.24.565.exe]], false, true))
print(winapi.error())

