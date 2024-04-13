static ZIPARCHIVE_METHOD(getArchiveComment)
{
	struct zip *intern;
	zval *this = getThis();
	long flags = 0;
	const char * comment;
	int comment_len = 0;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		return;
	}

	comment = zip_get_archive_comment(intern, &comment_len, (int)flags);
	if(comment==NULL) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((char *)comment, (long)comment_len, 1);
}