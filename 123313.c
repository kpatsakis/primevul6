httpd_compress(isc_httpd_t *httpd) {
	z_stream zstr;
	isc_region_t r;
	int ret;
	int inputlen;

	inputlen = isc_buffer_usedlength(&httpd->bodybuffer);
	alloc_compspace(httpd, inputlen);
	isc_buffer_region(&httpd->compbuffer, &r);

	/*
	 * We're setting output buffer size to input size so it fails if the
	 * compressed data size would be bigger than the input size.
	 */
	memset(&zstr, 0, sizeof(zstr));
	zstr.total_in = zstr.avail_in = zstr.total_out = zstr.avail_out =
		inputlen;

	zstr.next_in = isc_buffer_base(&httpd->bodybuffer);
	zstr.next_out = r.base;

	ret = deflateInit(&zstr, Z_DEFAULT_COMPRESSION);
	if (ret == Z_OK) {
		ret = deflate(&zstr, Z_FINISH);
	}
	deflateEnd(&zstr);
	if (ret == Z_STREAM_END) {
		isc_buffer_add(&httpd->compbuffer, inputlen - zstr.avail_out);
		return (ISC_R_SUCCESS);
	} else {
		return (ISC_R_FAILURE);
	}
}