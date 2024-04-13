php_apache_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	apr_size_t len, tlen=0;
	php_struct *ctx = SG(server_context);
	request_rec *r;
	apr_bucket_brigade *brigade;

	r = ctx->r;
	brigade = ctx->brigade;
	len = count_bytes;

	/*
	 * This loop is needed because ap_get_brigade() can return us partial data
	 * which would cause premature termination of request read. Therefor we
	 * need to make sure that if data is available we fill the buffer completely.
	 */

	while (ap_get_brigade(r->input_filters, brigade, AP_MODE_READBYTES, APR_BLOCK_READ, len) == APR_SUCCESS) {
		apr_brigade_flatten(brigade, buf, &len);
		apr_brigade_cleanup(brigade);
		tlen += len;
		if (tlen == count_bytes || !len) {
			break;
		}
		buf += len;
		len = count_bytes - tlen;
	}

	return tlen;
}