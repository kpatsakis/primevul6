static ZIPARCHIVE_METHOD(setCommentName)
{
	struct zip *intern;
	zval *this = getThis();
	int comment_len, name_len;
	char * comment, *name;
	int idx;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
			&name, &name_len, &comment, &comment_len) == FAILURE) {
		return;
	}

	if (name_len < 1) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Empty string as entry name");
	}

	idx = zip_name_locate(intern, name, 0);
	if (idx < 0) {
		RETURN_FALSE;
	}
	PHP_ZIP_SET_FILE_COMMENT(intern, idx, comment, comment_len);
}