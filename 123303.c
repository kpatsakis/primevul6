httpd_addheader(isc_httpd_t *httpd, const char *name, const char *val) {
	isc_result_t result;
	unsigned int needlen;

	REQUIRE(VALID_HTTPD(httpd));

	needlen = strlen(name); /* name itself */
	if (val != NULL) {
		needlen += 2 + strlen(val); /* :<space> and val */
	}
	needlen += 2; /* CRLF */

	while (isc_buffer_availablelength(&httpd->headerbuffer) < needlen) {
		result = grow_headerspace(httpd);
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
	}

	if (val != NULL) {
		return (isc_buffer_printf(&httpd->headerbuffer, "%s: %s\r\n",
					  name, val));
	} else {
		return (isc_buffer_printf(&httpd->headerbuffer, "%s\r\n",
					  name));
	}
}