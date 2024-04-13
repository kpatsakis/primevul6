void lua_push_nil_table_entry(lua_State *L, const char *key) {
  if(L) {
    lua_pushstring(L, key);
    lua_pushnil(L);
    lua_settable(L, -3);
  }
}