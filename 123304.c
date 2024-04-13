new_httpd(isc_httpdmgr_t *httpdmgr, isc_nmhandle_t *handle) {
	isc_httpd_t *httpd = NULL;
	char *headerdata = NULL;

	REQUIRE(VALID_HTTPDMGR(httpdmgr));

	httpd = isc_nmhandle_getdata(handle);
	if (httpd == NULL) {
		httpd = isc_nmhandle_getextra(handle);
		*httpd = (isc_httpd_t){ .handle = NULL };
		httpdmgr_attach(httpdmgr, &httpd->mgr);
	}

	if (httpd->handle == NULL) {
		isc_nmhandle_setdata(handle, httpd, httpd_reset, httpd_put);
		httpd->handle = handle;
	} else {
		INSIST(httpd->handle == handle);
	}

	/*
	 * Initialize the buffer for our headers.
	 */
	headerdata = isc_mem_get(httpdmgr->mctx, HTTP_SENDGROW);
	isc_buffer_init(&httpd->headerbuffer, headerdata, HTTP_SENDGROW);
	isc_buffer_clear(&httpd->headerbuffer);

	isc_buffer_initnull(&httpd->compbuffer);
	isc_buffer_clear(&httpd->compbuffer);

	isc_buffer_initnull(&httpd->bodybuffer);

	ISC_LINK_INIT(httpd, link);

	httpd->magic = HTTPD_MAGIC;
	httpd->state = RECV;

	LOCK(&httpdmgr->lock);
	ISC_LIST_APPEND(httpdmgr->running, httpd, link);
	UNLOCK(&httpdmgr->lock);

	isc_nmhandle_attach(httpd->handle, &httpd->readhandle);
	isc_nm_read(handle, httpd_request, httpdmgr);
}