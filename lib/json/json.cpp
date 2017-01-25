#include <taolua/stdafx.h>
#include <cmath>
#include "json.h"
#include "json11/json11.hpp"

namespace taolua {
namespace json {

class LuaJsonDecode
{
public:
    LuaJsonDecode(json11::Json val)
        : _val(val) { }

    void to_table(LuaWrapper& G) const
    {
        _G = &G;
        push(_val, false);
    }

protected:
    void push(const json11::Json& val, bool in_table) const
    {
        switch(val.type())
        {
        case json11::Json::ARRAY:
            _G->newtable((int)val.array_items().size());
            push_array(val.array_items());
            break;
        case json11::Json::OBJECT:
            _G->newtable(0, (int)val.object_items().size());
            push_object(val.object_items());
            break;
        default:
            push_plain(val);
            break;
        }
    }

    void push_plain(const json11::Json& val) const
    {
        switch(val.type())
        {
        case json11::Json::NUL:
            _G->push();
            break;

        case json11::Json::NUMBER:
        {
            double ip, fp;
            // http://stackoverflow.com/a/1521682/3628322
            fp = std::modf(val.number_value(), &ip);
            if(fp == 0.0) _G->push(val.int_value());
            else          _G->push(val.number_value());
            break;
        }
        case json11::Json::BOOL:
            _G->push(val.bool_value());
            break;

        case json11::Json::STRING:
            _G->push(val.string_value());
            break;

        default:
            _G->push();
            break;
        }
    }

    void push_array(const std::vector<json11::Json>& arr) const
    {
        size_t i = 1;
        for(const auto& a : arr) {
            push(a, true);
            _G->rawseti(-2, i);
            i++;
        }
    }

    void push_object(const std::map<std::string, json11::Json>& obj) const
    {
        for(const auto& o : obj) {
            push(o.second, true);
            _G->setfield(-2, o.first.c_str());
        }
    }

protected:
    json11::Json        _val;
    mutable LuaWrapper* _G;
};

LUAAPI(decode)
{
    DECL_WRAP;
    std::string errmsg;
    auto jsonstr = G.check_str_raw(1);
    auto jsonval = json11::Json::parse(jsonstr, errmsg);

    if(!errmsg.empty()) {
        G.push(false);
        G.push(errmsg);
    }
    else {
        G.push(true);
        G.push(LuaJsonDecode(jsonval));
    }

    return 2;
}

BEG_LIB_API()
    LIBAPI(decode)
END_LIB_API()

DECL_MODULE_MAGIC(__init__)
{

}

} }
