static int ntop_lua_require(lua_State* L)
{
  char *script_name;

  if(lua_type(L, 1) != LUA_TSTRING ||
     (script_name = (char*)lua_tostring(L, 1)) == NULL)
    return 0;

  lua_getglobal( L, "package" );
  lua_getfield( L, -1, "path" );

  string cur_path = lua_tostring( L, -1 ), parsed, script_path = "";
  stringstream input_stringstream(cur_path);
  while(getline(input_stringstream, parsed, ';')) {
    /* Example: package.path = dirs.installdir .. "/scripts/lua/modules/?.lua;" .. package.path */
    unsigned found = parsed.find_last_of("?");
    if(found) {
      string s = parsed.substr(0, found) + script_name + ".lua";
      if(Utils::file_exists(s.c_str())) {
	script_path = s;
	break;
      }
    }
  }

  if(script_path == "" ||
     __ntop_lua_handlefile(L, (char *)script_path.c_str(), false))
    return 0;

  return 1;
}