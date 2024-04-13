SPL_METHOD(SplObjectStorage, detach)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}
	spl_object_storage_detach(intern, getThis(), obj TSRMLS_CC);

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */