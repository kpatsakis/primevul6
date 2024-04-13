static PHP_NAMED_FUNCTION(zif_zip_entry_read)
{
	zval * zip_entry;
	long len = 0;
	zip_read_rsrc * zr_rsrc;
	char *buffer;
	int n = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zip_entry, &len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(zr_rsrc, zip_read_rsrc *, &zip_entry, -1, le_zip_entry_name, le_zip_entry);

	if (len <= 0) {
		len = 1024;
	}

	if (zr_rsrc->zf) {
		buffer = safe_emalloc(len, 1, 1);
		n = zip_fread(zr_rsrc->zf, buffer, len);
		if (n > 0) {
			buffer[n] = 0;
			RETURN_STRINGL(buffer, n, 0);
		} else {
			efree(buffer);
			RETURN_EMPTY_STRING()
		}
	} else {
		RETURN_FALSE;
	}
}