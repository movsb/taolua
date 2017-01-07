local fs = taolua.file_system

local files, folders = fs.get_folder_files('G:\\blog', true, true)

print('files:\n')

for k, v in ipairs(files) do
    print(k .. ': ' .. v)
end

print('folders:\n')

for k, v in ipairs(folders) do
    print(k .. ': ' .. v)
end

