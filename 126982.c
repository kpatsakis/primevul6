static const char **make_argv(lua_State * vm, u_int offset) {
  const char **argv;
  int i;
  int argc = lua_gettop(vm) - offset;

  if(!(argv = (const char**)calloc(argc, sizeof (char *))))
    /* raise an error and never return */
    luaL_error(vm, "Can't allocate memory for arguments array");

  /* fprintf(stderr, "%s\n", argv[0]); */
  for(i=0; i<argc; i++) {
    u_int idx = i + offset;
    /* accepts string or number */
    if(lua_isstring(vm, idx) || lua_isnumber(vm, idx)) {
      if(!(argv[i] = (char*)lua_tostring (vm, idx))) {
	/* raise an error and never return */
	luaL_error(vm, "Error duplicating string area for arg #%d", i);
      }
    } else {
      /* raise an error and never return */
      luaL_error(vm, "Invalid arg #%d: args must be strings or numbers", i);
    }

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "[%d] %s", i, argv[i]);
  }

  return(argv);
}