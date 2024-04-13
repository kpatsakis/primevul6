static void redirect_to_password_change(struct mg_connection *conn,
				    const struct mg_request_info *request_info) {
  char session_id[33], buf[128];

  mg_get_cookie(conn, "session", session_id, sizeof(session_id));
  ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] %s(%s)", __FUNCTION__, session_id);

    mg_printf(conn,
	      "HTTP/1.1 302 Found\r\n"
	      "Set-Cookie: session=%s; path=/;%s\r\n"  // Session ID
	      "Location: %s%s?referer=%s%s%s%s\r\n\r\n", /* FIX */
	      session_id,
	      get_secure_cookie_attributes(request_info),
	      ntop->getPrefs()->get_http_prefix(),
	      Utils::getURL((char*)CHANGE_PASSWORD_ULR, buf, sizeof(buf)),
	      mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
	      conn->request_info.uri,
	      conn->request_info.query_string ? "%3F" /* ? */: "",
	      conn->request_info.query_string ? conn->request_info.query_string : "");
}