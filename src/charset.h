#pragma once

namespace taolua
{

typedef std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> U8U16Cvt;

inline std::wstring from_utf8(const std::string& s)
{
    return U8U16Cvt().from_bytes(s);
}

inline std::string to_utf8(const std::wstring& s)
{
    return U8U16Cvt().to_bytes(s);
}

}
