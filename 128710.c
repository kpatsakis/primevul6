static ZIPARCHIVE_METHOD(setCommentIndex)
{
	struct zip *intern;
	zval *this = getThis();
	long index;
	int comment_len;
	char * comment;
	struct zip_stat sb;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls",
			&index, &comment, &comment_len) == FAILURE) {
		return;
	}

	PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	PHP_ZIP_SET_FILE_COMMENT(intern, index, comment, comment_len);
}