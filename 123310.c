destroy_httpdmgr(isc_httpdmgr_t *httpdmgr) {
	isc_httpdurl_t *url;

	isc_refcount_destroy(&httpdmgr->references);

	LOCK(&httpdmgr->lock);

	REQUIRE((httpdmgr->flags & ISC_HTTPDMGR_SHUTTINGDOWN) != 0);
	REQUIRE(ISC_LIST_EMPTY(httpdmgr->running));

	httpdmgr->magic = 0;

	if (httpdmgr->sock != NULL) {
		isc_nmsocket_close(&httpdmgr->sock);
	}

	/*
	 * Clear out the list of all actions we know about.  Just free the
	 * memory.
	 */
	url = ISC_LIST_HEAD(httpdmgr->urls);
	while (url != NULL) {
		isc_mem_free(httpdmgr->mctx, url->url);
		ISC_LIST_UNLINK(httpdmgr->urls, url, link);
		isc_mem_put(httpdmgr->mctx, url, sizeof(isc_httpdurl_t));
		url = ISC_LIST_HEAD(httpdmgr->urls);
	}

	UNLOCK(&httpdmgr->lock);
	isc_mutex_destroy(&httpdmgr->lock);

	if (httpdmgr->ondestroy != NULL) {
		(httpdmgr->ondestroy)(httpdmgr->cb_arg);
	}
	isc_mem_putanddetach(&httpdmgr->mctx, httpdmgr, sizeof(isc_httpdmgr_t));
}