static int ntop_get_local_networks(lua_State* vm) {
  lua_newtable(vm);
  ntop->getLocalNetworks(vm);

  return(CONST_LUA_OK);
}