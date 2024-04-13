static ZIPARCHIVE_METHOD(statName)
{
	struct zip *intern;
	zval *this = getThis();
	char *name;
	int name_len;
	long flags = 0;
	struct zip_stat sb;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|l",
			&name, &name_len, &flags) == FAILURE) {
		return;
	}

	PHP_ZIP_STAT_PATH(intern, name, name_len, flags, sb);

	RETURN_SB(&sb);
}