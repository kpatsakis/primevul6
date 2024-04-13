static ZIPARCHIVE_METHOD(locateName)
{
	struct zip *intern;
	zval *this = getThis();
	char *name;
	int name_len;
	long flags = 0;
	long idx = -1;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|l",
			&name, &name_len, &flags) == FAILURE) {
		return;
	}
	if (name_len<1) {
		RETURN_FALSE;
	}

	idx = (long)zip_name_locate(intern, (const char *)name, flags);

	if (idx >= 0) {
		RETURN_LONG(idx);
	} else {
		RETURN_FALSE;
	}
}