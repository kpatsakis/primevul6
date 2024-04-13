isc_httpdmgr_create(isc_nm_t *nm, isc_mem_t *mctx, isc_sockaddr_t *addr,
		    isc_httpdclientok_t *client_ok,
		    isc_httpdondestroy_t *ondestroy, void *cb_arg,
		    isc_httpdmgr_t **httpdmgrp) {
	isc_result_t result;
	isc_httpdmgr_t *httpdmgr = NULL;

	REQUIRE(nm != NULL);
	REQUIRE(mctx != NULL);
	REQUIRE(httpdmgrp != NULL && *httpdmgrp == NULL);

	httpdmgr = isc_mem_get(mctx, sizeof(isc_httpdmgr_t));
	*httpdmgr = (isc_httpdmgr_t){ .client_ok = client_ok,
				      .ondestroy = ondestroy,
				      .cb_arg = cb_arg,
				      .render_404 = render_404,
				      .render_500 = render_500 };

	isc_mutex_init(&httpdmgr->lock);
	isc_mem_attach(mctx, &httpdmgr->mctx);

	ISC_LIST_INIT(httpdmgr->running);
	ISC_LIST_INIT(httpdmgr->urls);

	isc_refcount_init(&httpdmgr->references, 1);

	CHECK(isc_nm_listentcp(nm, addr, httpd_newconn, httpdmgr,
			       sizeof(isc_httpd_t), 5, NULL, &httpdmgr->sock));

	httpdmgr->magic = HTTPDMGR_MAGIC;
	*httpdmgrp = httpdmgr;

	return (ISC_R_SUCCESS);

cleanup:
	httpdmgr->magic = 0;
	isc_refcount_decrementz(&httpdmgr->references);
	isc_refcount_destroy(&httpdmgr->references);
	isc_mem_detach(&httpdmgr->mctx);
	isc_mutex_destroy(&httpdmgr->lock);
	isc_mem_put(mctx, httpdmgr, sizeof(isc_httpdmgr_t));

	return (result);
}