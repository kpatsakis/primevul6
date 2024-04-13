static ZIPARCHIVE_METHOD(renameName)
{
	struct zip *intern;
	zval *this = getThis();
	struct zip_stat sb;
	char *name, *new_name;
	int name_len, new_name_len;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &name, &name_len, &new_name, &new_name_len) == FAILURE) {
		return;
	}

	if (new_name_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Empty string as new entry name");
		RETURN_FALSE;
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, 0, sb);

	if (zip_rename(intern, sb.index, (const char *)new_name)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}