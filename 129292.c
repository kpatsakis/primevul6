php_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	php_struct *ctx = SG(server_context);
	const apr_array_header_t *arr = apr_table_elts(ctx->r->subprocess_env);
	char *key, *val;
	int new_val_len;

	APR_ARRAY_FOREACH_OPEN(arr, key, val)
		if (!val) {
			val = "";
		}
		if (sapi_module.input_filter(PARSE_SERVER, key, &val, strlen(val), &new_val_len TSRMLS_CC)) {
			php_register_variable_safe(key, val, new_val_len, track_vars_array TSRMLS_CC);
		}
	APR_ARRAY_FOREACH_CLOSE()

	if (sapi_module.input_filter(PARSE_SERVER, "PHP_SELF", &ctx->r->uri, strlen(ctx->r->uri), &new_val_len TSRMLS_CC)) {
		php_register_variable_safe("PHP_SELF", ctx->r->uri, new_val_len, track_vars_array TSRMLS_CC);
	}
}