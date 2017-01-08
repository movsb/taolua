local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local ps = taolua.psapi
local pses = ps.processes()
if pses then
    print(ps.topsobj(13804):term())
    print(winapi.error())
end

