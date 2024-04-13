httpdmgr_detach(isc_httpdmgr_t **httpdmgrp) {
	isc_httpdmgr_t *httpdmgr = NULL;

	REQUIRE(httpdmgrp != NULL);
	REQUIRE(VALID_HTTPDMGR(*httpdmgrp));

	httpdmgr = *httpdmgrp;
	*httpdmgrp = NULL;

	if (isc_refcount_decrement(&httpdmgr->references) == 1) {
		destroy_httpdmgr(httpdmgr);
	}
}