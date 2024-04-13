int send_error(struct mg_connection *conn, int status, const char *reason, const char *fmt, ...) {
  va_list ap;

  conn->status_code = status;

  (void) mg_printf(conn,
		   "HTTP/1.1 %d %s\r\n"
		   "Content-Type: text/html\r\n"
		   "Connection: close\r\n"
		   "\r\n", status, reason);

  /* Errors 1xx, 204 and 304 MUST NOT send a body */
  if(status > 199 && status != 204 && status != 304) {
    char buf[BUFSIZ];
    int  len;

    conn->num_bytes_sent = 0;
    va_start(ap, fmt);
    len = mg_vsnprintf(conn, buf, sizeof(buf), fmt, ap);
    va_end(ap);
    conn->num_bytes_sent += mg_write(conn, buf, len);
    cry_connection(conn, buf);
  }

  return(1);
}