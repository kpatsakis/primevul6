static PHP_NAMED_FUNCTION(zif_zip_close)
{
	zval * zip;
	zip_rsrc *z_rsrc = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zip) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(z_rsrc, zip_rsrc *, &zip, -1, le_zip_dir_name, le_zip_dir);

	/* really close the zip will break BC :-D */
	zend_list_delete(Z_LVAL_P(zip));
}