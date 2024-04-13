static void redirect_to_ssl(struct mg_connection *conn,
                            const struct mg_request_info *request_info) {
  const char *host = mg_get_header(conn, "Host");
  //  u_int16_t port = ntop->get_HTTPserver()->get_port();

  if(host != NULL) {
    const char *p = strchr(host, ':');

    if(p)
      mg_printf(conn, "HTTP/1.1 302 Found\r\n"
		"Location: https://%.*s:%u/%s\r\n\r\n",
		(int) (p - host), host, ntop->getPrefs()->get_https_port(), request_info->uri);
    else
      mg_printf(conn, "HTTP/1.1 302 Found\r\n"
		"Location: https://%s:%u/%s\r\n\r\n",
		host, ntop->getPrefs()->get_https_port(), request_info->uri);
  } else {
    mg_printf(conn, "%s", "HTTP/1.1 500 Error\r\n\r\nHost: header is not set");
  }
}