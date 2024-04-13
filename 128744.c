static ZIPARCHIVE_METHOD(getStream)
{
	struct zip *intern;
	zval *this = getThis();
	struct zip_stat sb;
	char *filename;
	int	filename_len;
	char *mode = "rb";
	php_stream *stream;
	ze_zip_object *obj;

	if (!this) {
		RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}

	if (zip_stat(intern, filename, 0, &sb) != 0) {
		RETURN_FALSE;
	}

	obj = (ze_zip_object*) zend_object_store_get_object(this TSRMLS_CC);

	stream = php_stream_zip_open(obj->filename, filename, mode STREAMS_CC TSRMLS_CC);
	if (stream) {
		php_stream_to_zval(stream, return_value);
	}
}