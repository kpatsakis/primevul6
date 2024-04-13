static PHP_NAMED_FUNCTION(zif_zip_read)
{
	zval *zip_dp;
	zip_read_rsrc *zr_rsrc;
	int ret;
	zip_rsrc *rsrc_int;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zip_dp) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(rsrc_int, zip_rsrc *, &zip_dp, -1, le_zip_dir_name, le_zip_dir);

	if (rsrc_int && rsrc_int->za) {
		if (rsrc_int->index_current >= rsrc_int->num_files) {
			RETURN_FALSE;
		}

		zr_rsrc = emalloc(sizeof(zip_read_rsrc));

		ret = zip_stat_index(rsrc_int->za, rsrc_int->index_current, 0, &zr_rsrc->sb);

		if (ret != 0) {
			efree(zr_rsrc);
			RETURN_FALSE;
		}

		zr_rsrc->zf = zip_fopen_index(rsrc_int->za, rsrc_int->index_current, 0);
		if (zr_rsrc->zf) {
			rsrc_int->index_current++;
			ZEND_REGISTER_RESOURCE(return_value, zr_rsrc, le_zip_entry);
		} else {
			efree(zr_rsrc);
			RETURN_FALSE;
		}

	} else {
		RETURN_FALSE;
	}
}