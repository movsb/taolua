local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local ps = taolua.psapi

local ok, pses = ps.list_processes()
if ok then
    for pid, info in pairs(pses) do
        printf('pid: %d, ppid: %d, name: %s', pid, info.ppid, info.name)
    end
end


