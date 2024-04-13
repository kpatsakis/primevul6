static int checkInformativeCaptive(const struct mg_connection *conn,
				   const struct mg_request_info *request_info) {
#ifdef NTOPNG_PRO
#ifdef DEBUG
  char buf[32];

  ntop->getTrace()->traceEvent(TRACE_NORMAL, "[CAPTIVE] @ %s/%08X [Redirecting to %s%s]",
			       Utils::intoaV4((unsigned int)conn->request_info.remote_ip, buf, sizeof(buf)),
			       (unsigned int)conn->request_info.remote_ip,
			       mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
			       request_info->uri);
#endif

  if(!ntop->addToNotifiedInformativeCaptivePortal(htonl((unsigned int)conn->request_info.remote_ip)))
    return(0);

  /* Success */
  return(1);
#endif

  return(0);
}