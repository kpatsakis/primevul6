static ZIPARCHIVE_METHOD(renameIndex)
{
	struct zip *intern;
	zval *this = getThis();

	char *new_name;
	int new_name_len;
	long index;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &index, &new_name, &new_name_len) == FAILURE) {
		return;
	}

	if (index < 0) {
		RETURN_FALSE;
	}

	if (new_name_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Empty string as new entry name");
		RETURN_FALSE;
	}
	if (zip_rename(intern, index, (const char *)new_name) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}