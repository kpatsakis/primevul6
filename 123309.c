alloc_compspace(isc_httpd_t *httpd, unsigned int size) {
	char *newspace = NULL;
	isc_region_t r;

	isc_buffer_region(&httpd->compbuffer, &r);
	if (size < r.length) {
		return;
	}

	newspace = isc_mem_get(httpd->mgr->mctx, size);
	isc_buffer_reinit(&httpd->compbuffer, newspace, size);

	if (r.base != NULL) {
		isc_mem_put(httpd->mgr->mctx, r.base, r.length);
	}
}