static int ntop_send_udp_data(lua_State* vm) {
  int rc, port, sockfd = ntop->getUdpSock();
  char *host, *data;

  if(sockfd == -1)
    return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  host = (char*)lua_tostring(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  port = (u_int16_t)lua_tonumber(vm, 2);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_ERROR);
  data = (char*)lua_tostring(vm, 3);

  if(strchr(host, ':') != NULL) {
    struct sockaddr_in6 server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, host, &server_addr.sin6_addr);
    server_addr.sin6_port = htons(port);

    rc = sendto(sockfd, data, strlen(data),0,
		(struct sockaddr *)&server_addr,
		sizeof(server_addr));
  } else {
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host); /* FIX: add IPv6 support */
    server_addr.sin_port = htons(port);

    rc = sendto(sockfd, data, strlen(data),0,
		(struct sockaddr *)&server_addr,
		sizeof(server_addr));
  }

  if(rc == -1)
    return(CONST_LUA_ERROR);
  else
    return(CONST_LUA_OK);
}