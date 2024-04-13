static void php_zip_get_from(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	struct zip *intern;
	zval *this = getThis();

	struct zip_stat sb;
	struct zip_file *zf;

	char *filename;
	int	filename_len;
	long index = -1;
	long flags = 0;
	long len = 0;

	char *buffer;
	int n = 0;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (type == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p|ll", &filename, &filename_len, &len, &flags) == FAILURE) {
			return;
		}
		PHP_ZIP_STAT_PATH(intern, filename, filename_len, flags, sb);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|ll", &index, &len, &flags) == FAILURE) {
			return;
		}
		PHP_ZIP_STAT_INDEX(intern, index, 0, sb);
	}

	if (sb.size < 1) {
		RETURN_EMPTY_STRING();
	}

	if (len < 1) {
		len = sb.size;
	}
	if (index >= 0) {
		zf = zip_fopen_index(intern, index, flags);
	} else {
		zf = zip_fopen(intern, filename, flags);
	}

	if (zf == NULL) {
		RETURN_FALSE;
	}

	buffer = safe_emalloc(len, 1, 2);
	n = zip_fread(zf, buffer, len);
	if (n < 1) {
		efree(buffer);
		RETURN_EMPTY_STRING();
	}

	zip_fclose(zf);
	buffer[n] = 0;
	RETURN_STRINGL(buffer, n, 0);
}