httpd_response(isc_httpd_t *httpd) {
	isc_result_t result;
	unsigned int needlen;

	REQUIRE(VALID_HTTPD(httpd));

	needlen = strlen(httpd->protocol) + 1; /* protocol + space */
	needlen += 3 + 1; /* room for response code, always 3 bytes */
	needlen += strlen(httpd->retmsg) + 2; /* return msg + CRLF */

	while (isc_buffer_availablelength(&httpd->headerbuffer) < needlen) {
		result = grow_headerspace(httpd);
		if (result != ISC_R_SUCCESS) {
			return (result);
		}
	}

	return (isc_buffer_printf(&httpd->headerbuffer, "%s %03u %s\r\n",
				  httpd->protocol, httpd->retcode,
				  httpd->retmsg));
}