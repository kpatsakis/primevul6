httpd_put(void *arg) {
	isc_httpd_t *httpd = (isc_httpd_t *)arg;
	isc_httpdmgr_t *mgr = NULL;

	REQUIRE(VALID_HTTPD(httpd));

	mgr = httpd->mgr;
	REQUIRE(VALID_HTTPDMGR(mgr));

	httpd->magic = 0;
	httpd->mgr = NULL;

	free_buffer(mgr->mctx, &httpd->headerbuffer);
	free_buffer(mgr->mctx, &httpd->compbuffer);
	httpdmgr_detach(&mgr);

#if ENABLE_AFL
	if (finishhook != NULL) {
		finishhook();
	}
#endif /* ENABLE_AFL */
}