httpd_newconn(isc_nmhandle_t *handle, isc_result_t result, void *arg) {
	isc_httpdmgr_t *httpdmgr = (isc_httpdmgr_t *)arg;
	isc_sockaddr_t peeraddr;

	REQUIRE(VALID_HTTPDMGR(httpdmgr));

	if ((httpdmgr->flags & ISC_HTTPDMGR_SHUTTINGDOWN) != 0) {
		return (ISC_R_CANCELED);
	} else if (result == ISC_R_CANCELED) {
		isc_httpdmgr_shutdown(&httpdmgr);
		return (result);
	} else if (result != ISC_R_SUCCESS) {
		return (result);
	}

	peeraddr = isc_nmhandle_peeraddr(handle);
	if (httpdmgr->client_ok != NULL &&
	    !(httpdmgr->client_ok)(&peeraddr, httpdmgr->cb_arg))
	{
		return (ISC_R_FAILURE);
	}

	new_httpd(httpdmgr, handle);

	return (ISC_R_SUCCESS);
}