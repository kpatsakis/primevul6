void lua_push_str_table_entry(lua_State *L, const char *key, char *value) {
  if(L) {
    lua_pushstring(L, key);
    lua_pushstring(L, value);
    lua_settable(L, -3);
  }
}