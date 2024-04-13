static void redirect_to_login(struct mg_connection *conn,
                              const struct mg_request_info *request_info,
			      const char *referer) {
  char session_id[33], buf[128];

  if(isCaptiveConnection(conn)) {
    if(referer)
      mg_printf(conn,
		"HTTP/1.1 302 Found\r\n"
		"Set-Cookie: session=%s; path=/; expires=Thu, 01-Jan-1970 00:00:01 GMT; max-age=0;%s\r\n"  // Session ID
		"Location: %s%s?referer=%s\r\n\r\n", /* FIX */
		session_id,
		get_secure_cookie_attributes(request_info),
		ntop->getPrefs()->get_http_prefix(), ntop->getPrefs()->getCaptivePortalUrl(), referer);
    else
      mg_printf(conn,
		"HTTP/1.1 302 Found\r\n"
		"Set-Cookie: session=%s; path=/; expires=Thu, 01-Jan-1970 00:00:01 GMT; max-age=0;%s\r\n"  // Session ID
		"Location: %s%s\r\n\r\n", /* FIX */
		session_id,
		get_secure_cookie_attributes(request_info),
		ntop->getPrefs()->get_http_prefix(), ntop->getPrefs()->getCaptivePortalUrl());
  } else {
#ifdef DEBUG
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "[LOGIN] [Host: %s][URI: %s]",
				 mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
				 request_info->uri);
#endif

    mg_get_cookie(conn, "session", session_id, sizeof(session_id));
    ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] %s(%s)", __FUNCTION__, session_id);

    if(referer)
      mg_printf(conn,
		"HTTP/1.1 302 Found\r\n"
		"Set-Cookie: session=%s; path=/; expires=Thu, 01-Jan-1970 00:00:01 GMT; max-age=0;%s\r\n"  // Session ID
		"Location: %s%s?referer=%s\r\n\r\n",
		session_id,
		get_secure_cookie_attributes(request_info),
		ntop->getPrefs()->get_http_prefix(),
		Utils::getURL((char*)LOGIN_URL, buf, sizeof(buf)), referer);
    else
      mg_printf(conn,
	      "HTTP/1.1 302 Found\r\n"
	      "Set-Cookie: session=%s; path=/; expires=Thu, 01-Jan-1970 00:00:01 GMT; max-age=0;%s\r\n"  // Session ID
	      "Location: %s%s\r\n\r\n",
	      session_id,
	      get_secure_cookie_attributes(request_info),
	      ntop->getPrefs()->get_http_prefix(),
	      Utils::getURL((char*)LOGIN_URL, buf, sizeof(buf)));
  }
}