httpd_addheaderuint(isc_httpd_t *httpd, const char *name, int val) {
	isc_result_t result;
	unsigned int needlen;
	char buf[sizeof "18446744073709551616"];

	REQUIRE(VALID_HTTPD(httpd));

	snprintf(buf, sizeof(buf), "%d", val);

	needlen = strlen(name);	    /* name itself */
	needlen += 2 + strlen(buf); /* :<space> and val */
	needlen += 2;		    /* CRLF */

	while (isc_buffer_availablelength(&httpd->headerbuffer) < needlen) {
		result = grow_headerspace(httpd);
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
	}

	return (isc_buffer_printf(&httpd->headerbuffer, "%s: %s\r\n", name,
				  buf));
}