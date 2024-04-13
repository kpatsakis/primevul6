static int ntop_rrd_create(lua_State* vm) {
  const char *filename;
  unsigned long pdp_step;
  const char **argv;
  int argc, status, offset = 3;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((filename = (const char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  pdp_step = (unsigned long)lua_tonumber(vm, 2);

  ntop->getTrace()->traceEvent(TRACE_INFO, "%s(%s)", __FUNCTION__, filename);

  argc = lua_gettop(vm) - offset;
  argv = make_argv(vm, offset);

  reset_rrd_state();
  status = rrd_create_r(filename, pdp_step, time(NULL)-86400 /* 1 day */, argc, argv);
  free(argv);

  if(status != 0) {
    char *err = rrd_get_error();

    if(err != NULL) {
      luaL_error(vm, err);
      return(CONST_LUA_ERROR);
    }
  }

  return(CONST_LUA_OK);
}