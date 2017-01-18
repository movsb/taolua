#pragma once

#include <cassert>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <functional>
#include <codecvt>
#include <memory>
#include <regex>

#include <tchar.h>
#include <Windows.h>

extern "C" {
#include <lua/luaconf.h>
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "lua.h"
#include "charset.h"
#include "winhlp.h"
