static int handle_lua_request(struct mg_connection *conn) {
  struct mg_request_info *request_info = mg_get_request_info(conn);
  char *crlf;
  u_int len;
  char username[33] = { 0 };
  char *referer = (char*)mg_get_header(conn, "Referer");
  u_int8_t whitelisted, authorized;

  if(referer == NULL)
    referer = (char*)"";

  if((crlf = strstr(request_info->uri, "\r\n")))
    *crlf = '\0'; /* Prevents HTTP splitting attacks */

  len = (u_int)strlen(request_info->uri);

#ifdef HAVE_NEDGE
  if(!ntop->getPro()->has_valid_license()) {
    if (! ntop->getGlobals()->isShutdown()) {
      ntop->getTrace()->traceEvent(TRACE_NORMAL, "License expired, shutting down...");
      ntop->getGlobals()->shutdown();
      ntop->shutdown();
    }
  }
#endif

#ifdef DEBUG
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "[Host: %s][URI: %s][%s][Referer: %s]",
			       mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
			       request_info->uri,
			       request_info->query_string ? request_info->query_string : "",
			       (char*)mg_get_header(conn, "Referer"));
#endif

  if((ntop->getGlobals()->isShutdown())
     //|| (strcmp(request_info->request_method, "GET"))
     || (ntop->getRedis() == NULL /* Starting up... */)
     || (ntop->get_HTTPserver() == NULL))
    return(send_error(conn, 403 /* Forbidden */, request_info->uri,
		      "Unexpected HTTP method or ntopng still starting up..."));

#ifdef DEBUG
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "################# [HTTP] %s [%s]",
			       request_info->uri, referer);
#endif

#ifdef HAVE_MYSQL
  if(ntop->getPrefs()->do_dump_flows_on_mysql()
     && !MySQLDB::isDbCreated()
     && strcmp(request_info->uri, PLEASE_WAIT_URL)) {
    redirect_to_please_wait(conn, request_info);
  } else
#endif
#ifndef HAVE_NEDGE
  if(ntop->get_HTTPserver()->is_ssl_enabled()
     && (!request_info->is_ssl)
     && isCaptiveURL(request_info->uri)
     && (!strstr(referer, HOTSPOT_DETECT_LUA_URL))
     && (!strstr(referer, ntop->getPrefs()->getCaptivePortalUrl()))
     // && ((mg_get_header(conn, "Host") == NULL) || (mg_get_header(conn, "Host")[0] == '\0'))
     ) {
    redirect_to_ssl(conn, request_info);
    return(1);
  } else
#endif
  if(!strcmp(request_info->uri, HOTSPOT_DETECT_URL)) {
    mg_printf(conn, "HTTP/1.1 302 Found\r\n"
	      "Expires: 0\r\n"
	      "Cache-Control: no-store, no-cache, must-revalidate\t\n"
	      "Pragma: no-cache\r\n"
	      "Location: http://%s%s%s%s\r\n\r\n",
	      mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
	      HOTSPOT_DETECT_LUA_URL,
	      request_info->query_string ? "?" : "",
	      request_info->query_string ? request_info->query_string : "");
    return(1);
  }
#if 0
 else if(!strcmp(request_info->uri, KINDLE_WIFISTUB_URL)) {
    mg_printf(conn, "HTTP/1.1 302 Found\r\n"
	      "Expires: 0\r\n"
	      "Cache-Control: no-store, no-cache, must-revalidate\t\n"
	      "Pragma: no-cache\r\n"
	      "Referer: %s\r\n"
	      "Location: http://%s%s%s%s\r\n\r\n",
	      request_info->uri,
	      mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
	      ntop->getPrefs()->getCaptivePortalUrl(),
	      request_info->query_string ? "?" : "",
	      request_info->query_string ? request_info->query_string : "");
    return(1);
  }
#endif

  whitelisted = isWhitelistedURI(request_info->uri);
  authorized = is_authorized(conn, request_info, username, sizeof(username));

  if(isStaticResourceUrl(request_info, len))
    ;
  else if((!whitelisted) && (!authorized)) {
    if(conn->client.lsa.sin.sin_port == ntop->get_HTTPserver()->getSplashPort())
      mg_printf(conn,
		"HTTP/1.1 302 Found\r\n"
		"Location: %s%s?referer=%s\r\n\r\n",
		ntop->getPrefs()->get_http_prefix(), BANNED_SITE_URL,
		mg_get_header(conn, "Host"));
    else if(strcmp(request_info->uri, NETWORK_LOAD_URL) == 0) {
      // avoid sending login redirect to allow js itself to redirect the user
      return(send_error(conn, 403 /* Forbidden */, request_info->uri, "Login Required"));
    } else {
      char referer[255];

      snprintf(referer, sizeof(referer), "%s%s%s%s",
	      mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
	      conn->request_info.uri,
	      conn->request_info.query_string ? "%3F" /* ? */: "",
	      conn->request_info.query_string ? conn->request_info.query_string : "");

      redirect_to_login(conn, request_info, referer);
    }

    return(1);
  } else if ((strcmp(request_info->uri, CHANGE_PASSWORD_ULR) != 0)
      && (strcmp(request_info->uri, LOGOUT_URL) != 0)
	     && authorized
      && ntop->mustChangePassword(username)) {
    redirect_to_password_change(conn, request_info);
    return(1);
  } else if(strcmp(request_info->uri, AUTHORIZE_URL) == 0) {
    authorize(conn, request_info, username);
    return(1);
  }

#ifdef DEBUG
  ntop->getTrace()->traceEvent(TRACE_WARNING, "Username = %s", username);
#endif

  if(strstr(request_info->uri, "//")
     || strstr(request_info->uri, "&&")
     || strstr(request_info->uri, "??")
     || strstr(request_info->uri, "..")
     || strstr(request_info->uri, "\r")
     || strstr(request_info->uri, "\n")
     ) {
    ntop->getTrace()->traceEvent(TRACE_WARNING, "[HTTP] The URL %s is invalid/dangerous",
				 request_info->uri);
    return(send_error(conn, 400 /* Bad Request */, request_info->uri,
		      "The URL specified contains invalid/dangerous characters"));
  }

  if((strncmp(request_info->uri, "/lua/", 5) == 0)
     || (strcmp(request_info->uri, "/metrics") == 0)
     || (strcmp(request_info->uri, "/") == 0)) {
    /* Lua Script */
    char path[255] = { 0 }, uri[2048];
    struct stat buf;
    bool found;

    if(strstr(request_info->uri, "/lua/pro")
       && (!ntop->getPrefs()->is_pro_edition())) {
      return(send_error(conn, 403 /* Forbidden */, request_info->uri,
			"Professional edition license required"));
    }

    if(strstr(request_info->uri, "/lua/pro/enterprise")
       && (!ntop->getPrefs()->is_enterprise_edition())) {
      return(send_error(conn, 403 /* Forbidden */, request_info->uri,
			"Enterprise edition license required"));
    }

    if((!whitelisted)
       && isCaptiveConnection(conn)
       && (!isCaptiveURL(request_info->uri))) {
      redirect_to_login(conn, request_info, (referer[0] == '\0') ? NULL : referer);
      return(0);
    } else {
      if(strcmp(request_info->uri, "/metrics") == 0)
	snprintf(path, sizeof(path), "%s/lua/metrics.lua",
	  httpserver->get_scripts_dir());
      else
	snprintf(path, sizeof(path), "%s%s%s",
	       httpserver->get_scripts_dir(),
	       Utils::getURL(len == 1 ? (char*)"/lua/index.lua" : request_info->uri, uri, sizeof(uri)),
	       len > 1 && request_info->uri[len-1] == '/' ? (char*)"index.lua" : (char*)"");

      if(strlen(path) > 4 && strncmp(&path[strlen(path) - 4], ".lua", 4))
	snprintf(&path[strlen(path)], sizeof(path) - strlen(path) - 1, "%s", (char*)".lua");

      ntop->fixPath(path);
      found = ((stat(path, &buf) == 0) && (S_ISREG(buf.st_mode))) ? true : false;
    }

    if(found) {
      LuaEngine *l = new LuaEngine();

      ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] %s [%s]", request_info->uri, path);

      if(l == NULL) {
	ntop->getTrace()->traceEvent(TRACE_ERROR, "[HTTP] Unable to start Lua interpreter");
	return(send_error(conn, 500 /* Internal server error */,
			  "Internal server error", "%s", "Unable to start Lua interpreter"));
      } else {
	l->handle_script_request(conn, request_info, path);
	delete l;
	return(1); /* Handled */
      }
    }

    uri_encode(request_info->uri, uri, sizeof(uri)-1);

    return(send_error(conn, 404, "Not Found", PAGE_NOT_FOUND, uri));
  } else {
    /* Prevent short URI or .inc files to be served */
    if((len < 4) || (strncmp(&request_info->uri[len-4], ".inc", 4) == 0)) {
      return(send_error(conn, 403, "Forbidden",
			ACCESS_FORBIDDEN, request_info->uri));
    } else {
      ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] Serving file %s%s",
				   ntop->get_HTTPserver()->get_docs_dir(), request_info->uri);
      request_info->query_string = ""; /* Discard things like ?v=4.4.0 */
      return(0); /* This is a static document so let mongoose handle it */
    }
  }
}