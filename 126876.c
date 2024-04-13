static int ntop_post_http_json_data(lua_State* vm) {
  char *username, *password, *url, *json;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((username = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((password = (char*)lua_tostring(vm, 2)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((url = (char*)lua_tostring(vm, 3)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 4, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((json = (char*)lua_tostring(vm, 4)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(Utils::postHTTPJsonData(username, password, url, json))
    return(CONST_LUA_OK);
  else
    return(CONST_LUA_ERROR);
}