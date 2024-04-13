Lua::~Lua() {
  if(L) lua_close(L);
}