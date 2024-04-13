static ZIPARCHIVE_METHOD(statIndex)
{
	struct zip *intern;
	zval *this = getThis();
	long index, flags = 0;

	struct zip_stat sb;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l",
			&index, &flags) == FAILURE) {
		return;
	}

	if (zip_stat_index(intern, index, flags, &sb) != 0) {
		RETURN_FALSE;
	}
	RETURN_SB(&sb);
}