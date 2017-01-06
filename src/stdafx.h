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

#include <Windows.h>

extern "C" {
#include <lua/luaconf.h>
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
}

#include "charset.h"
