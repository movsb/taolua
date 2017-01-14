#include "stdafx.h"
#include <wininet.h>
#include "network.h"
#include <regex>

#pragma comment(lib, "wininet.lib")

namespace taolua {
namespace network {

constexpr const wchar_t* const g_user_agent = L"taolua/1.0 Firefox/50.0";

struct INetHandleBooler { bool operator()(HINTERNET h) const { return h != nullptr; } };
struct INetHandleCloser { bool operator()(HINTERNET h) const { return !!::InternetCloseHandle(h); } };
typedef AutoCloseT<HINTERNET, INetHandleBooler, INetHandleCloser> AutoINetHandle;

LUAAPI(urlopen)
{
    DECL_WRAP;
    auto uri = G.check_str(1);
    AutoINetHandle hInternet = ::InternetOpen(g_user_agent, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if(hInternet) {
        AutoINetHandle hResource = ::InternetOpenUrl(hInternet, uri.c_str(), nullptr, 0,
            INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI,
            0);
        if(hResource) {
            struct HTTPResponse
            {
                std::wstring ver;
                int          code;
                std::wstring reason;
                WStr2WStr    hdrs;
                std::string  body;

                void to_table(LuaWrapper& G) const
                {
                    G.newtable(0, 5);
                    G.setfield("ver",    ver);
                    G.setfield("code",   code);
                    G.setfield("reason", reason);
                    G.setfield("hdrs",   hdrs);
                    G.setfield("body",   body);
                }
            };

            HTTPResponse res;            

            // https://msdn.microsoft.com/en-us/library/windows/desktop/aa385373(v=vs.85).aspx
            auto get_hdrs = [&] {
                BoolVal br;
                DWORD size = 0;
                std::unique_ptr<wchar_t[]> buf;

                while(!(br = ::HttpQueryInfo(hResource, HTTP_QUERY_RAW_HEADERS, buf.get(), &size, nullptr))) {
                    if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                        buf.reset(new wchar_t[size]);
                        continue;
                    }
                    else {
                        break;
                    }
                }

                if(buf) {
                    // auto str = std::string(buf.get(), size);
                    try {
                        std::wcmatch matches;
                        const wchar_t* ptr = buf.get();

                        auto next = [&ptr] { while(*ptr++); return ptr; };

                        if(std::regex_search(ptr, matches, std::wregex(LR"([\t ]*(HTTP/\d+\.\d+)[\t ]+(\d+)[\t ]+(.*))", std::regex_constants::icase))) {
                            res.ver     = matches[1].str();
                            res.code    = _wtoi(matches[2].first);
                            res.reason  = matches[3].str();
                        }
                        else {
                            throw "bad ver";
                        }

                        while(*next()) {
                            if(std::regex_search(ptr, matches, std::wregex(LR"([\t ]*([\w-]+)[\t ]*:[\t ]*(.*)[\t ]*)"))) {
                                auto k = matches[1].str();
                                auto v = matches[2].str();
                                res.hdrs.emplace(k, v);
                            }
                            else {
                                throw "bad key-values";
                            }
                        }
                    }
                    catch(...) {
                        br = false;
                    }
                }

                return br && buf;
            };

            auto get_body = [&] {
                BoolVal br;
                DWORD read;
                constexpr size_t buf_size = 32 * 1024;
                auto buf = std::make_unique<char[]>(buf_size);
                while((br = ::InternetReadFile(hResource, buf.get(), buf_size, &read)) && read)
                    res.body.append(buf.get(), read);
                return br;
            };

            if(get_hdrs() && get_body()) {
                G.push(true);
                G.push(res);
                return 2;
            }
            else {
                G.push(false);
                return 1;
            }
        }
    }

    G.push(false);
    return 1;
}

BEG_LIB_API()
    LIBAPI(urlopen)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

}}
