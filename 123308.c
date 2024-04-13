httpdmgr_attach(isc_httpdmgr_t *source, isc_httpdmgr_t **targetp) {
	REQUIRE(VALID_HTTPDMGR(source));
	REQUIRE(targetp != NULL && *targetp == NULL);

	isc_refcount_increment(&source->references);

	*targetp = source;
}