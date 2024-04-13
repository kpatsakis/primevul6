SPL_METHOD(SplObjectStorage, contains)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}
	RETURN_BOOL(spl_object_storage_contains(intern, getThis(), obj TSRMLS_CC));
} /* }}} */