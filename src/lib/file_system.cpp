#include "stdafx.h"
#include "file_system.h"

#include <shlwapi.h>

// BEG_LIB_NAMESPACE(file_system)
namespace taolua { namespace file_system {

void GetFolderFiles(const std::wstring& path, const std::wstring& cur,
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
                        GetFolderFiles(root, name, alsosub, files, folders);
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

bool (CreateSymbolicLink)(LPCWSTR dst, LPCWSTR src, bool is_dir)
{
    return !!::CreateSymbolicLink(dst, src, is_dir ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0);
}

//////////////////////////////////////////////////////////////////////////

LUAAPI(get_folder_files)
{
    DECL_WRAP;

    std::vector<std::wstring> files;
    std::vector<std::wstring> folders;

    auto path = G.check_str(1);
    auto sub = G.check_bool(2);
    auto merge = G.opt_bool(3, false);

    GetFolderFiles(path, L"", sub, &files, merge ? &files : &folders);

    G.newtable(files);
    if(!merge)
        G.newtable(folders);

    return merge ? 1 : 2;
}

LUAAPI(ln)
{
    DECL_WRAP;
    MyStr dst = G.check_str(1);
    MyStr src = G.check_str(2);
    auto is_dir = G.check_bool(3);
    auto overwrite = G.opt_bool(4, false);

    if(overwrite && ::PathFileExists(dst)) {
        DWORD attr = ::GetFileAttributes(dst);
        BoolVal b;
        if(attr != INVALID_FILE_ATTRIBUTES && attr & FILE_ATTRIBUTE_DIRECTORY)
            b = ::RemoveDirectory(dst);
        else
            b = ::DeleteFile(dst);
        if(!b) {
            SAVE_LAST_ERROR;
            G.push(b);
            return 1;
        }
    }

    auto ok = (CreateSymbolicLink)(dst, src, is_dir);
    SAVE_LAST_ERROR;
    G.push(ok);
    return 1;
}

BEG_LIB_API()
    LIBAPI(get_folder_files)
    LIBAPI(ln)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

END_LIB_NAMESPACE()
