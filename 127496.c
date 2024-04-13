static int ntop_reload_l7_rules(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) {
    char *net;
    AddressTree ptree;

    if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
    if((net = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);

#ifdef SHAPER_DEBUG
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s(%s)", __FUNCTION__, net);
#endif

    ptree.addAddresses(net);

#ifdef NTOPNG_PRO
    ntop_interface->refreshL7Rules(&ptree);
#endif

    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}