static ZIPARCHIVE_METHOD(setArchiveComment)
{
	struct zip *intern;
	zval *this = getThis();
	int comment_len;
	char * comment;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &comment, &comment_len) == FAILURE) {
		return;
	}
	if (zip_set_archive_comment(intern, (const char *)comment, (int)comment_len)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}