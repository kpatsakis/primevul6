isc_httpdmgr_addurl(isc_httpdmgr_t *httpdmgr, const char *url, bool isstatic,
		    isc_httpdaction_t *func, void *arg) {
	isc_httpdurl_t *item;

	REQUIRE(VALID_HTTPDMGR(httpdmgr));

	if (url == NULL) {
		httpdmgr->render_404 = func;
		return (ISC_R_SUCCESS);
	}

	item = isc_mem_get(httpdmgr->mctx, sizeof(isc_httpdurl_t));

	item->url = isc_mem_strdup(httpdmgr->mctx, url);

	item->action = func;
	item->action_arg = arg;
	item->isstatic = isstatic;
	isc_time_now(&item->loadtime);

	ISC_LINK_INIT(item, link);

	LOCK(&httpdmgr->lock);
	ISC_LIST_APPEND(httpdmgr->urls, item, link);
	UNLOCK(&httpdmgr->lock);

	return (ISC_R_SUCCESS);
}