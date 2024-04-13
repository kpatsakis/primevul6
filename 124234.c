static int isCaptiveConnection(struct mg_connection *conn) {
  char *host = (char*)mg_get_header(conn, "Host");

  return(ntop->getPrefs()->isCaptivePortalEnabled()
	 && (ntohs(conn->client.lsa.sin.sin_port) == 80
	     || ntohs(conn->client.lsa.sin.sin_port) == 443)
	 && (ntop->getPrefs()->get_alt_http_port() != 0)
	 && host
	 && (strcasestr(host, CONST_HELLO_HOST) == NULL)
	 );
}