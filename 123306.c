grow_headerspace(isc_httpd_t *httpd) {
	char *newspace = NULL;
	unsigned int newlen;
	isc_region_t r;

	isc_buffer_region(&httpd->headerbuffer, &r);
	newlen = r.length + HTTP_SENDGROW;
	if (newlen > HTTP_SEND_MAXLEN) {
		return (ISC_R_NOSPACE);
	}

	newspace = isc_mem_get(httpd->mgr->mctx, newlen);

	isc_buffer_reinit(&httpd->headerbuffer, newspace, newlen);

	isc_mem_put(httpd->mgr->mctx, r.base, r.length);

	return (ISC_R_SUCCESS);
}