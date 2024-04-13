static ZIPARCHIVE_METHOD(getNameIndex)
{
	struct zip *intern;
	zval *this = getThis();
	const char *name;
	long flags = 0, index = 0;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l",
			&index, &flags) == FAILURE) {
		return;
	}

	name = zip_get_name(intern, (int) index, flags);

	if (name) {
		RETVAL_STRING((char *)name, 1);
	} else {
		RETURN_FALSE;
	}
}