static ZIPARCHIVE_METHOD(open)
{
	struct zip *intern;
	char *filename;
	int filename_len;
	int err = 0;
	long flags = 0;
	char resolved_path[MAXPATHLEN];

	zval *this = getThis();
	ze_zip_object *ze_obj = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|l", &filename, &filename_len, &flags) == FAILURE) {
		return;
	}

	if (this) {
		/* We do not use ZIP_FROM_OBJECT, zip init function here */
		ze_obj = (ze_zip_object*) zend_object_store_get_object(this TSRMLS_CC);
	}

	if (filename_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string as source");
		RETURN_FALSE;
	}

	if (ZIP_OPENBASEDIR_CHECKPATH(filename)) {
		RETURN_FALSE;
	}

	if (!expand_filepath(filename, resolved_path TSRMLS_CC)) {
		RETURN_FALSE;
	}

	if (ze_obj->za) {
		/* we already have an opened zip, free it */
		if (zip_close(ze_obj->za) != 0) {
			_zip_free(ze_obj->za);
		}
		ze_obj->za = NULL;
	}
	if (ze_obj->filename) {
		efree(ze_obj->filename);
		ze_obj->filename = NULL;
	}

	intern = zip_open(resolved_path, flags, &err);
	if (!intern || err) {
		RETURN_LONG((long)err);
	}
	ze_obj->filename = estrdup(resolved_path);
	ze_obj->filename_len = strlen(resolved_path);
	ze_obj->za = intern;
	RETURN_TRUE;
}