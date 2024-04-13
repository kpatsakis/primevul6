httpd_endheaders(isc_httpd_t *httpd) {
	isc_result_t result;

	REQUIRE(VALID_HTTPD(httpd));

	while (isc_buffer_availablelength(&httpd->headerbuffer) < 2) {
		result = grow_headerspace(httpd);
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
	}

	return (isc_buffer_printf(&httpd->headerbuffer, "\r\n"));
}