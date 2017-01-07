#pragma once

namespace taolua {
namespace file_system {

DECL_METHODS;

void get_folder_files(const std::wstring& folder, const std::wstring& cur,
    bool alsosub, std::vector<std::wstring>* files, std::vector<std::wstring>* folders);

}}
