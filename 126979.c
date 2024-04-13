void lua_push_float_table_entry(lua_State *L, const char *key, float value) {
  if(L) {
    lua_pushstring(L, key);
    lua_pushnumber(L, value);
    lua_settable(L, -3);
  }
}