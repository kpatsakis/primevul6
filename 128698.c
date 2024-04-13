static PHP_NAMED_FUNCTION(zif_zip_entry_open)
{
	zval * zip;
	zval * zip_entry;
	char *mode = NULL;
	int mode_len = 0;
	zip_read_rsrc * zr_rsrc;
	zip_rsrc *z_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr|s", &zip, &zip_entry, &mode, &mode_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(zr_rsrc, zip_read_rsrc *, &zip_entry, -1, le_zip_entry_name, le_zip_entry);
	ZEND_FETCH_RESOURCE(z_rsrc, zip_rsrc *, &zip, -1, le_zip_dir_name, le_zip_dir);

	if (zr_rsrc->zf != NULL) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}