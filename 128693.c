static ZIPARCHIVE_METHOD(close)
{
	struct zip *intern;
	zval *this = getThis();
	ze_zip_object *ze_obj;

	if (!this) {
			RETURN_FALSE;
	}

	ZIP_FROM_OBJECT(intern, this);

	ze_obj = (ze_zip_object*) zend_object_store_get_object(this TSRMLS_CC);

	if (zip_close(intern)) {
		RETURN_FALSE;
	}

	efree(ze_obj->filename);
	ze_obj->filename = NULL;
	ze_obj->filename_len = 0;
	ze_obj->za = NULL;

	RETURN_TRUE;
}