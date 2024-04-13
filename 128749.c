static PHP_NAMED_FUNCTION(zif_zip_entry_close)
{
	zval * zip_entry;
	zip_read_rsrc * zr_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zip_entry) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(zr_rsrc, zip_read_rsrc *, &zip_entry, -1, le_zip_entry_name, le_zip_entry);

	RETURN_BOOL(SUCCESS == zend_list_delete(Z_LVAL_P(zip_entry)));
}