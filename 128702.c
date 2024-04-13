static ZIPARCHIVE_METHOD(unchangeIndex)
{
	struct zip *intern;
	zval *this = getThis();
	long index;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == FAILURE) {
		return;
	}

	if (index < 0) {
		RETURN_FALSE;
	}

	if (zip_unchange(intern, index) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}