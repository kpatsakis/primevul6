static ZIPARCHIVE_METHOD(unchangeName)
{
	struct zip *intern;
	zval *this = getThis();
	struct zip_stat sb;
	char *name;
	int name_len;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name_len < 1) {
		RETURN_FALSE;
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, 0, sb);

	if (zip_unchange(intern, sb.index) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}