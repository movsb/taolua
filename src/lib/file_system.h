#pragma once

BEG_LIB_NAMESPACE(file_system)

DECL_MODULE;

void get_folder_files(const std::wstring& folder, const std::wstring& cur,
    bool alsosub, std::vector<std::wstring>* files, std::vector<std::wstring>* folders);

END_LIB_NAMESPACE()
