php_apache_sapi_header_handler(sapi_header_struct *sapi_header, sapi_header_op_enum op, sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	php_struct *ctx;
	char *val, *ptr;

	ctx = SG(server_context);

	switch (op) {
		case SAPI_HEADER_DELETE:
			apr_table_unset(ctx->r->headers_out, sapi_header->header);
			return 0;

		case SAPI_HEADER_DELETE_ALL:
			apr_table_clear(ctx->r->headers_out);
			return 0;

		case SAPI_HEADER_ADD:
		case SAPI_HEADER_REPLACE:
			val = strchr(sapi_header->header, ':');

			if (!val) {
				return 0;
			}
			ptr = val;

			*val = '\0';

			do {
				val++;
			} while (*val == ' ');

			if (!strcasecmp(sapi_header->header, "content-type")) {
				if (ctx->content_type) {
					efree(ctx->content_type);
				}
				ctx->content_type = estrdup(val);
			} else if (!strcasecmp(sapi_header->header, "content-length")) {
#ifdef PHP_WIN32
# ifdef APR_HAS_LARGE_FILES
				ap_set_content_length(ctx->r, (apr_off_t) _strtoui64(val, (char **)NULL, 10));
# else
				ap_set_content_length(ctx->r, (apr_off_t) strtol(val, (char **)NULL, 10));
# endif
#else
				ap_set_content_length(ctx->r, (apr_off_t) strtol(val, (char **)NULL, 10));
#endif
			} else if (op == SAPI_HEADER_REPLACE) {
				apr_table_set(ctx->r->headers_out, sapi_header->header, val);
			} else {
				apr_table_add(ctx->r->headers_out, sapi_header->header, val);
			}

			*ptr = ':';

			return SAPI_HEADER_ADD;

		default:
			return 0;
	}
}