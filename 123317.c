httpd_reset(void *arg) {
	isc_httpd_t *httpd = (isc_httpd_t *)arg;
	isc_httpdmgr_t *httpdmgr = NULL;

	REQUIRE(VALID_HTTPD(httpd));

	httpdmgr = httpd->mgr;

	REQUIRE(VALID_HTTPDMGR(httpdmgr));

	LOCK(&httpdmgr->lock);
	ISC_LIST_UNLINK(httpdmgr->running, httpd, link);
	UNLOCK(&httpdmgr->lock);

	httpd->recvbuf[0] = 0;
	httpd->recvlen = 0;
	httpd->consume = 0;
	httpd->truncated = false;
	httpd->headers = NULL;
	httpd->method = METHOD_UNKNOWN;
	httpd->url = NULL;
	httpd->querystring = NULL;
	httpd->protocol = NULL;
	httpd->flags = 0;

	isc_buffer_clear(&httpd->headerbuffer);
	isc_buffer_clear(&httpd->compbuffer);
	isc_buffer_invalidate(&httpd->bodybuffer);
}