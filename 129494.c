SPL_METHOD(SplObjectStorage, offsetGet)
{
	zval *obj;
	spl_SplObjectStorageElement *element;
	spl_SplObjectStorage *intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *hash;
	int hash_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &obj) == FAILURE) {
		return;
	}

	hash = spl_object_storage_get_hash(intern, getThis(), obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return;
	}

	element = spl_object_storage_get(intern, hash, hash_len TSRMLS_CC);
	spl_object_storage_free_hash(intern, hash);

	if (!element) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Object not found");
	} else {
		RETURN_ZVAL(element->inf,1, 0);
	}
} /* }}} */