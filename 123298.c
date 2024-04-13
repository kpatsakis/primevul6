isc_httpdmgr_shutdown(isc_httpdmgr_t **httpdmgrp) {
	isc_httpdmgr_t *httpdmgr;
	isc_httpd_t *httpd;

	REQUIRE(httpdmgrp != NULL);
	REQUIRE(VALID_HTTPDMGR(*httpdmgrp));

	httpdmgr = *httpdmgrp;
	*httpdmgrp = NULL;

	isc_nm_stoplistening(httpdmgr->sock);

	LOCK(&httpdmgr->lock);
	httpdmgr->flags |= ISC_HTTPDMGR_SHUTTINGDOWN;

	httpd = ISC_LIST_HEAD(httpdmgr->running);
	while (httpd != NULL) {
		isc_nm_cancelread(httpd->readhandle);
		httpd = ISC_LIST_NEXT(httpd, link);
	}
	UNLOCK(&httpdmgr->lock);

	httpdmgr_detach(&httpdmgr);
}