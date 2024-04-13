SPL_METHOD(SplObjectStorage, addAll)
{
	zval *obj;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage *)zend_object_store_get_object(getThis() TSRMLS_CC);
	spl_SplObjectStorage *other;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &obj, spl_ce_SplObjectStorage) == FAILURE) {
		return;
	}

	other = (spl_SplObjectStorage *)zend_object_store_get_object(obj TSRMLS_CC);

	spl_object_storage_addall(intern, getThis(),  other TSRMLS_CC);

	RETURN_LONG(zend_hash_num_elements(&intern->storage));
} /* }}} */