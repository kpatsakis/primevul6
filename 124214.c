static void authorize(struct mg_connection *conn,
                      const struct mg_request_info *request_info,
		      char *username) {
  char user[32] = { '\0' }, password[32] = { '\0' }, referer[256] = { '\0' };

  if(!strcmp(request_info->request_method, "POST")) {
    char post_data[1024];
    int post_data_len = mg_read(conn, post_data, sizeof(post_data));

    mg_get_var(post_data, post_data_len, "user", user, sizeof(user));
    mg_get_var(post_data, post_data_len, "password", password, sizeof(password));
    mg_get_var(post_data, post_data_len, "referer", referer, sizeof(referer));
  } else {
    // Fetch user name and password.
    get_qsvar(request_info, "user", user, sizeof(user));
    get_qsvar(request_info, "password", password, sizeof(password));
    get_qsvar(request_info, "ref", referer, sizeof(referer));

    if(referer[0] == '\0') {
      for(int i=0; request_info->http_headers[i].name != NULL; i++) {
	if(strcmp(request_info->http_headers[i].name, "Referer") == 0) {
	  snprintf(referer, sizeof(referer), "%s", request_info->http_headers[i].value);
	  break;
	}
      }
    }
  }

  if(isCaptiveConnection(conn) || ntop->isCaptivePortalUser(user) ||
	    (!ntop->checkUserPassword(user, password))) {
    // Authentication failure, redirect to login
    redirect_to_login(conn, request_info, (referer[0] == '\0') ? NULL : referer);
  } else {
    /* Referer url must begin with '/' */
    if((referer[0] != '/') || (strcmp(referer, AUTHORIZE_URL) == 0)) {
      char *r = strchr(referer, '/');
      strcpy(referer, r ? r : "/");
    }

    set_cookie(conn, user, referer);
  }
}