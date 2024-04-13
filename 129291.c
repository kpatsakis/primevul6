php_apache_sapi_flush(void *server_context)
{
	php_struct *ctx;
	request_rec *r;
	TSRMLS_FETCH();

	ctx = server_context;

	/* If we haven't registered a server_context yet,
	 * then don't bother flushing. */
	if (!server_context) {
		return;
	}

	r = ctx->r;

	sapi_send_headers(TSRMLS_C);

	r->status = SG(sapi_headers).http_response_code;
	SG(headers_sent) = 1;

	if (ap_rflush(r) < 0 || r->connection->aborted) {
		php_handle_aborted_connection();
	}
}