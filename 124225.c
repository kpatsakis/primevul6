static int handle_http_message(const struct mg_connection *conn, const char *message) {
  ntop->getTrace()->traceEvent(TRACE_ERROR, "[HTTP] %s", message);
  return 1;
}