static int is_authorized(const struct mg_connection *conn,
                         const struct mg_request_info *request_info,
			 char *username, u_int username_len) {
  char session_id[33], buf[128];
  char key[64], user[32];
  char password[32];
  const char *auth_header_p;
  string auth_type = "", auth_string = "";
  bool user_login_disabled = !ntop->getPrefs()->is_users_login_enabled() ||
    authorized_localhost_users_login_disabled(conn);

#ifdef DEBUG
  ntop->getTrace()->traceEvent(TRACE_WARNING, "[AUTHORIZATION] [%s][%s]",
			       request_info->uri, request_info->query_string ? request_info->query_string : "");
#endif

  /*
    iOS / MacOS
    1. HOTSPOT_DETECT_URL        "/hotspot-detect.html"
    2. HOTSPOT_DETECT_LUA_URL    "/lua/hotspot-detect.lua"
    3. CAPTIVE_PORTAL_URL        "/lua/captive_portal.lua"
    4. AUTHORIZE_CAPTIVE_LUA_URL "/lua/authorize_captive.lua"
    5. logged in

    Kindle
    1. KINDLE_WIFISTUB_URL
  */
  if(!strcmp(request_info->uri, AUTHORIZE_CAPTIVE_LUA_URL)) {
    /* A captive portal request has been issued to the authorization url */

    if(ntop->getPrefs()->isInformativeCaptivePortalEnabled()) {
      /* If the captive portal is just informative, there's no need to check
	 any username or password. The request per se means the internet user
	 has accepted the 'terms of service'. */
      return(checkInformativeCaptive(conn, request_info));
    } else {
      /* Here the captive portal is not just informative; it requires authentication.
         For this reason it is necessary to check submitted username and password. */
      if(request_info->query_string) {
	get_qsvar(request_info, "username", username, username_len);
	get_qsvar(request_info, "password", password, sizeof(password));
      }

      return(ntop->checkUserPassword(username, password)
	     && checkCaptive(conn, request_info, username, password));
    }
  }

  if(checkGrafana(conn, request_info) == 1) {
    return(1);
  }

  if(user_login_disabled) {
    mg_get_cookie(conn, "user", username, username_len);
    if(strncmp(username, NTOP_NOLOGIN_USER, username_len)) {
      set_cookie(conn, (char *)NTOP_NOLOGIN_USER, NULL);
    }
    return 1;
  }

  /* Try to decode Authorization header if present */
  auth_header_p = mg_get_header(conn, "Authorization");
  string auth_header = auth_header_p ? auth_header_p  : "";
  istringstream iss(auth_header);
  getline(iss, auth_type, ' ');
  if(auth_type == "Basic") {
    string decoded_auth, user_s = "", pword_s = "";
    /* In case auth type is Basic, info are encoded in base64 */
    getline(iss, auth_string, ' ');
    decoded_auth = Utils::base64_decode(auth_string);
    istringstream authss(decoded_auth);
    getline(authss, user_s, ':');
    getline(authss, pword_s, ':');

    return ntop->checkUserPassword(user_s.c_str(), pword_s.c_str());
  }

  mg_get_cookie(conn, "user", username, username_len);
  mg_get_cookie(conn, "session", session_id, sizeof(session_id));

  if(!strcmp(username, NTOP_NOLOGIN_USER) && !user_login_disabled)
    /* Trying to access web interface with nologin after ntopng restart
       with different settings */
    return 0;

  if(session_id[0] == '\0') {
    /* Last resort: see if we have a user and password matching */
    mg_get_cookie(conn, "password", password, sizeof(password));

    return(ntop->checkUserPassword(username, password));
  }

  // ntop->getTrace()->traceEvent(TRACE_WARNING, "[HTTP] Received session %s/%s", session_id, username);

  snprintf(key, sizeof(key), CONST_RUNTIME_IS_AUTOLOGOUT_ENABLED);
  ntop->getRedis()->get(key, buf, sizeof(buf), true);
  // do_auto_logout() is the getter for the command-line specified
  // preference that defaults to true (i.e., auto_logout is enabled by default)
  // If do_auto_logout() is disabled, then the runtime auto logout preference
  // is taken into account.
  // If do_auto_logout() is false, then the auto logout is disabled regardless
  // of runtime preferences.
  if(ntop->getPrefs()->do_auto_logout() && strncmp(buf, (char*)"1", 1) == 0) {
    snprintf(key, sizeof(key), "sessions.%s", session_id);
    if((ntop->getRedis()->get(key, user, sizeof(user), true) < 0)
       || strcmp(user, username) /* Users don't match */) {
      ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] Session %s/%s is expired or empty user",
				   session_id, username);
      return(0);
    } else {
      ntop->getRedis()->expire(key, HTTP_SESSION_DURATION); /* Extend session */
      ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] Session %s is OK: extended for %u sec",
				   session_id, HTTP_SESSION_DURATION);
      return(1);
    }
  } else
    return(1);
}