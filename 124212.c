static int checkCaptive(const struct mg_connection *conn,
			const struct mg_request_info *request_info,
			char *username, char *password) {
#ifdef NTOPNG_PRO
  if(ntop->getPrefs()->isCaptivePortalEnabled()
     && ntop->isCaptivePortalUser(username)) {
    /*
      This user logged onto ntopng via the captive portal
    */
    u_int16_t host_pool_id;
    int32_t limited_lifetime = -1; /* Unlimited by default */
    char label[128];

    get_qsvar(request_info, "label", label, sizeof(label));

#ifdef DEBUG
    char buf[32];

    ntop->getTrace()->traceEvent(TRACE_NORMAL, "[CAPTIVE] %s @ %s/%08X [Redirecting to %s%s]",
				 username, Utils::intoaV4((unsigned int)conn->request_info.remote_ip, buf, sizeof(buf)),
				 (unsigned int)conn->request_info.remote_ip,
				 mg_get_header(conn, "Host") ? mg_get_header(conn, "Host") : (char*)"",
				 request_info->uri);
#endif

    char bridge_interface[32];

    if(!ntop->getUserAllowedIfname(username, bridge_interface, sizeof(bridge_interface)))
      return(0);

    ntop->getUserHostPool(username, &host_pool_id);
    ntop->hasUserLimitedLifetime(username, &limited_lifetime);

    if(!ntop->addIPToLRUMatches(htonl((unsigned int)conn->request_info.remote_ip),
			    host_pool_id, label, limited_lifetime, bridge_interface))
      return(0);

    /* Success */
    return(1);
  }
#endif

  return(0);
}