void lua_push_int32_table_entry(lua_State *L, const char *key, int32_t value) {
  if(L) {
    lua_pushstring(L, key);
    lua_pushnumber(L, (lua_Number)value);
    lua_settable(L, -3);
  }
}