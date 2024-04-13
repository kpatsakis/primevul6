static ZIPARCHIVE_METHOD(addEmptyDir)
{
	struct zip *intern;
	zval *this = getThis();
	char *dirname;
	int   dirname_len;
	int idx;
	struct zip_stat sb;
	char *s;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
				&dirname, &dirname_len) == FAILURE) {
		return;
	}

	if (dirname_len<1) {
		RETURN_FALSE;
	}

	if (dirname[dirname_len-1] != '/') {
		s=(char *)emalloc(dirname_len+2);
		strcpy(s, dirname);
		s[dirname_len] = '/';
		s[dirname_len+1] = '\0';
	} else {
		s = dirname;
	}

	idx = zip_stat(intern, s, 0, &sb);
	if (idx >= 0) {
		RETVAL_FALSE;
	} else {
		if (zip_add_dir(intern, (const char *)s) == -1) {
			RETVAL_FALSE;
		}
		RETVAL_TRUE;
	}

	if (s != dirname) {
		efree(s);
	}
}