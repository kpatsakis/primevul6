php_apache_sapi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	const char *env_var;

	if (ctx == NULL) {
		return NULL;
	}

	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
}