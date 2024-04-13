static int ntop_get_ndpi_interface_stats(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  nDPIStats stats;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) {
    ntop_interface->getnDPIStats(&stats);

    lua_newtable(vm);
    stats.lua(ntop_interface, vm);
  }

  return(CONST_LUA_OK);
}