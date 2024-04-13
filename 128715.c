static PHP_NAMED_FUNCTION(zif_zip_open)
{
	char     *filename;
	int       filename_len;
	char resolved_path[MAXPATHLEN + 1];
	zip_rsrc *rsrc_int;
	int err = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (filename_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as source");
		RETURN_FALSE;
	}

	if (ZIP_OPENBASEDIR_CHECKPATH(filename)) {
		RETURN_FALSE;
	}

	if(!expand_filepath(filename, resolved_path TSRMLS_CC)) {
		RETURN_FALSE;
	}

	rsrc_int = (zip_rsrc *)emalloc(sizeof(zip_rsrc));

	rsrc_int->za = zip_open(resolved_path, 0, &err);
	if (rsrc_int->za == NULL) {
		efree(rsrc_int);
		RETURN_LONG((long)err);
	}

	rsrc_int->index_current = 0;
	rsrc_int->num_files = zip_get_num_files(rsrc_int->za);

	ZEND_REGISTER_RESOURCE(return_value, rsrc_int, le_zip_dir);
}