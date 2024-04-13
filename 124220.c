HTTPserver::~HTTPserver() {
  if(httpd_v4) mg_stop(httpd_v4);

  free(docs_dir), free(scripts_dir);
  ntop->getTrace()->traceEvent(TRACE_NORMAL, "HTTP server terminated");
};