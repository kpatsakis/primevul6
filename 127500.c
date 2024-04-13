static int ntop_get_interface_flows_peers(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_name;
  u_int16_t vlan_id = 0;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TSTRING) {
    char buf[64];

    get_host_vlan_info((char*)lua_tostring(vm, 1), &host_name, &vlan_id, buf, sizeof(buf));
  } else
    host_name = NULL;

  /* Optional VLAN id */
  if(lua_type(vm, 2) == LUA_TNUMBER) vlan_id = (u_int16_t)lua_tonumber(vm, 2);

  if(ntop_interface)
    ntop_interface->getFlowPeersList(vm, get_allowed_nets(vm), host_name, vlan_id);

  return(CONST_LUA_OK);
}