static void redirect_to_please_wait(struct mg_connection *conn,
				    const struct mg_request_info *request_info) {
  char session_id[33], buf[128];

  mg_get_cookie(conn, "session", session_id, sizeof(session_id));
  ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] %s(%s)", __FUNCTION__, session_id);

  mg_printf(conn,
	    "HTTP/1.1 302 Found\r\n"
	    // "HTTP/1.1 401 Unauthorized\r\n"
	    // "WWW-Authenticate: Basic\r\n"
	    "Set-Cookie: session=%s; path=/; expires=Thu, 01-Jan-1970 00:00:01 GMT; max-age=0;%s\r\n"  // Session ID
	    "Location: %s%s?referer=%s%s%s\r\n\r\n",
	    session_id,
	    get_secure_cookie_attributes(request_info),
	    ntop->getPrefs()->get_http_prefix(),
	    Utils::getURL((char*)PLEASE_WAIT_URL, buf, sizeof(buf)),
	    conn->request_info.uri,
	    conn->request_info.query_string ? "%3F" /* ? */: "",
	    conn->request_info.query_string ? conn->request_info.query_string : "");
}