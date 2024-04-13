php_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	request_rec *r;
	php_struct *ctx;

	ctx = SG(server_context);
	r = ctx->r;

	if (ap_rwrite(str, str_length, r) < 0) {
		php_handle_aborted_connection();
	}

	return str_length; /* we always consume all the data passed to us. */
}