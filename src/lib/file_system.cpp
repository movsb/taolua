#include "stdafx.h"
#include "file_system.h"

namespace taolua {
namespace file_system {

class GC
{
public:
    GC() : i(9) {}

public:
    BEG_OBJ_API(GC, name)
        OBJAPI(__gc)
    END_OBJ_API()

    LIBAPI(__gc)
    {
        DECL_THIS;
        std::cout << "doing __gc: " << G.i << " ..." << std::endl;
        return 0;
    }

protected:
    int i;
};

//////////////////////////////////////////////////////////////////////////

LIBAPI(get_folder_files)
{
    DECL_WRAP;

    std::vector<std::wstring> files;
    std::vector<std::wstring> folders;

    auto path = G.check_str(1);
    auto sub = G.check_bool(2);
    auto merge = G.opt_bool(3, false);

    get_folder_files(path, L"", sub, &files, merge ? &files : &folders);

    G.newtable(files);
    if(!merge)
        G.newtable(folders);

    return merge ? 1 : 2;
}

BEG_LUA_API()
    LUAAPI(get_folder_files)
END_LUA_API()

//////////////////////////////////////////////////////////////////////////

void get_folder_files(const std::wstring& path, const std::wstring& cur,
    bool alsosub, std::vector<std::wstring>* files, std::vector<std::wstring>* folders)
{
    WIN32_FIND_DATA fd;
    HANDLE hFind;

    std::wstring root = path;
    std::wstring pattern;
    if(!root.empty() && (root.back() == L'\\' || root.back() == L'/'))
        root.pop_back();

    if(!cur.empty())
        pattern = root + L'\\' + cur + L"\\*.*";
    else
        pattern = root + L"\\*.*";

    hFind = ::FindFirstFile(pattern.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::wstring name(fd.cFileName);
                if(name != L"." && name != L"..") {
                    if(!cur.empty())
                        name = cur + L'\\' + name;
                    if(alsosub) {
                        folders->push_back(name);
                        get_folder_files(root, name, alsosub, files, folders);
                    }
                    else {
                        folders->emplace_back(std::move(name));
                    }
                }
            }
            else {
                std::wstring name;
                if(!cur.empty()) name = cur + L'\\' + fd.cFileName;
                else name = fd.cFileName;
                files->emplace_back(std::move(name));
            }
        } while(::FindNextFile(hFind, &fd));

        ::FindClose(hFind);
    }
}

}

}
