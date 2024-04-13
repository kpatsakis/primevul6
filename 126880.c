static int __ntop_lua_handlefile(lua_State* L, char *script_path, bool ex)
{
  int rc;
  LuaHandler *lh = new LuaHandler(L, script_path);

  rc = lh->luaL_dofileM(ex);
  delete lh;
  return rc;
}