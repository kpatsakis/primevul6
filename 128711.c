static ZIPARCHIVE_METHOD(deleteName)
{
	struct zip *intern;
	zval *this = getThis();
	int name_len;
	char *name;
	struct zip_stat sb;

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
	if (zip_delete(intern, sb.index)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}