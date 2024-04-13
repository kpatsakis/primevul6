static int ntop_lua_dofile(lua_State* L)
{
  char *script_path;

  if(lua_type(L, 1) != LUA_TSTRING ||
     (script_path = (char*)lua_tostring(L, 1)) == NULL ||
     __ntop_lua_handlefile(L, script_path, true))
    return 0;

  return 1;
}