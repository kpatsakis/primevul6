static void set_cookie(const struct mg_connection *conn,
                       char *user, char *referer) {
  char key[256], session_id[64], random[64];

  if(!strcmp(mg_get_request_info((struct mg_connection*)conn)->uri, "/metrics")
     || !strncmp(mg_get_request_info((struct mg_connection*)conn)->uri, GRAFANA_URL, strlen(GRAFANA_URL))
     || !strncmp(mg_get_request_info((struct mg_connection*)conn)->uri, POOL_MEMBERS_ASSOC_URL, strlen(POOL_MEMBERS_ASSOC_URL)))
    return;

  if(authorized_localhost_users_login_disabled(conn))
    return;
  
  // Authentication success:
  //   1. create new session
  //   2. set session ID token in the cookie
  //
  // The most secure way is to stay HTTPS all the time. However, just to
  // show the technique, we redirect to HTTP after the successful
  // authentication. The danger of doing this is that session cookie can
  // be stolen and an attacker may impersonate the user.
  // Secure application must use HTTPS all the time.

  snprintf(random, sizeof(random), "%d", rand());

  generate_session_id(session_id, random, user);

  // ntop->getTrace()->traceEvent(TRACE_ERROR, "==> %s\t%s", random, session_id);

  /* http://en.wikipedia.org/wiki/HTTP_cookie */
  mg_printf((struct mg_connection *)conn, "HTTP/1.1 302 Found\r\n"
	    "Set-Cookie: session=%s; path=/; max-age=%u;%s\r\n"  // Session ID
	    "Set-Cookie: user=%s; path=/; max-age=%u;%s\r\n"  // Set user, needed by JavaScript code
	    "Location: %s%s\r\n\r\n",
	    session_id, HTTP_SESSION_DURATION, get_secure_cookie_attributes(mg_get_request_info((struct mg_connection*)conn)),
	    user, HTTP_SESSION_DURATION, get_secure_cookie_attributes(mg_get_request_info((struct mg_connection*)conn)),
	    ntop->getPrefs()->get_http_prefix(), referer ? referer : "/");

  /* Save session in redis */
  snprintf(key, sizeof(key), "sessions.%s", session_id);
  ntop->getRedis()->set(key, user, HTTP_SESSION_DURATION);
  ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] Set session sessions.%s", session_id);
}