void lua_push_int_table_entry(lua_State *L, const char *key, u_int64_t value) {
  if(L) {
    lua_pushstring(L, key);
    /* using LUA_NUMBER (double: 64 bit) in place of LUA_INTEGER (ptrdiff_t: 32 or 64 bit
     * according to the platform, as defined in luaconf.h) to handle big counters */
    lua_pushnumber(L, (lua_Number)value);
    lua_settable(L, -3);
  }
}