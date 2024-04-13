void lua_push_bool_table_entry(lua_State *L, const char *key, bool value) {
  if(L) {
    lua_pushstring(L, key);
    lua_pushboolean(L, value ? 1 : 0);
    lua_settable(L, -3);
  }
}