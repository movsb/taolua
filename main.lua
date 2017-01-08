local printf = function(fmt, ...)
    return print(string.format(fmt, ...))
end

local ps = taolua.psapi

local pses = ps.processes()
for pid, info in pairs(pses or {}) do
    local psobj = ps.topsobj(pid)
    printf('%s, %s', psobj, info.name)
    local modules = psobj:modules()
    for _, m in ipairs(modules or {}) do
        printf('pid: %d, base: %x, size: %d, handle: %x, name: %s, path: %s',
            m.pid, m.base, m.size, m.handle, m.name, m.path)
    end
end


