/* Goxel 3D voxels editor
 *
 * copyright (c) 2017 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma GCC diagnostic push
#ifdef __clang__
#   pragma GCC diagnostic ignored "-Wstring-plus-int"
#endif

// Include first to fix windows compilation.
#include "../ext_src/lua/lprefix.h"

#include "luagoxel.h"

// Include all lua sources here.
#include "../ext_src/lua/lundump.c" // Keep on top to fix windows compilation

#include "../ext_src/lua/lapi.c"
#include "../ext_src/lua/lauxlib.c"
#include "../ext_src/lua/lbaselib.c"
#include "../ext_src/lua/lbitlib.c"
#include "../ext_src/lua/lcode.c"
#include "../ext_src/lua/lcorolib.c"
#include "../ext_src/lua/lctype.c"
#include "../ext_src/lua/ldblib.c"
#include "../ext_src/lua/ldebug.c"
#include "../ext_src/lua/ldo.c"
#include "../ext_src/lua/ldump.c"
#include "../ext_src/lua/lfunc.c"
#include "../ext_src/lua/lgc.c"
#include "../ext_src/lua/linit.c"
#include "../ext_src/lua/liolib.c"
#include "../ext_src/lua/llex.c"
#include "../ext_src/lua/lmathlib.c"
#include "../ext_src/lua/lmem.c"
#include "../ext_src/lua/loadlib.c"
#include "../ext_src/lua/lobject.c"
#include "../ext_src/lua/lopcodes.c"
#include "../ext_src/lua/loslib.c"
#include "../ext_src/lua/lparser.c"
#include "../ext_src/lua/lstate.c"
#include "../ext_src/lua/lstring.c"
#include "../ext_src/lua/lstrlib.c"
#include "../ext_src/lua/ltable.c"
#include "../ext_src/lua/ltablib.c"
#include "../ext_src/lua/ltm.c"
#include "../ext_src/lua/lutf8lib.c"
#include "../ext_src/lua/lvm.c"
#include "../ext_src/lua/lzio.c"

#pragma GCC diagnostic pop

void *luaG_checkpointer(lua_State *l, int narg, const char *type)
{
    void *ret;
    if (lua_islightuserdata(l, narg))
        return (void*)lua_topointer(l, narg);
    luaL_checktype(l, narg, LUA_TTABLE);
    lua_pushstring(l, "data");
    lua_rawget(l, narg);
    ret = (void*)lua_topointer(l, -1);
    lua_pop(l, -1);
    return ret;
}

int luaG_checkpos(lua_State *l, int idx, int pos[3])
{
    int i;
    luaL_checktype(l, idx, LUA_TTABLE);
    lua_pushvalue(l, idx);
    for (i = 0; i < 3; i++) {
        lua_pushinteger(l, i + 1);
        lua_gettable(l, -2);
        pos[i] = luaL_checkinteger(l, -1);
        lua_pop(l, 1);
    }
    lua_pop(l, 1);
    return 0;
}

int luaG_checkcolor(lua_State *l, int idx, uint8_t color[4])
{
    int i;
    luaL_checktype(l, idx, LUA_TTABLE);
    lua_pushvalue(l, idx);
    for (i = 0; i < 4; i++) {
        lua_pushinteger(l, i + 1);
        lua_gettable(l, -2);
        color[i] = luaL_checkinteger(l, -1);
        lua_pop(l, 1);
    }
    lua_pop(l, 1);
    return 0;
}

int luaG_checkaabb(lua_State *l, int idx, int aabb[2][3])
{
    luaG_checkpos(l, idx, aabb[1]);
    aabb[0][0] = 0;
    aabb[0][1] = 0;
    aabb[0][2] = 0;
    return 0;
}

void luaG_newintarray(lua_State *l, int n, const int *v)
{
    int i;
    lua_newtable(l);
    for (i = 0; i < n; i++) {
        lua_pushinteger(l, v[i]);
        lua_rawseti(l, -2, i + 1);
    }
}
