php_apache_sapi_read_cookies(TSRMLS_D)
{
	php_struct *ctx = SG(server_context);
	const char *http_cookie;

	http_cookie = apr_table_get(ctx->r->headers_in, "cookie");

	/* The SAPI interface should use 'const char *' */
	return (char *) http_cookie;
}