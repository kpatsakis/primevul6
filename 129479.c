SPL_METHOD(MultipleIterator, attachIterator)
{
	spl_SplObjectStorage        *intern;
	zval                        *iterator = NULL, *info = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|z!", &iterator, zend_ce_iterator, &info) == FAILURE) {
		return;
	}

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (info != NULL) {
		spl_SplObjectStorageElement *element;
		zval                         compare_result;

		if (Z_TYPE_P(info) != IS_LONG && Z_TYPE_P(info) != IS_STRING) {
			zend_throw_exception(spl_ce_InvalidArgumentException, "Info must be NULL, integer or string", 0 TSRMLS_CC);
			return;
		}

		zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
		while (zend_hash_get_current_data_ex(&intern->storage, (void**)&element, &intern->pos) == SUCCESS) {
			is_identical_function(&compare_result, info, element->inf TSRMLS_CC);
			if (Z_LVAL(compare_result)) {
				zend_throw_exception(spl_ce_InvalidArgumentException, "Key duplication error", 0 TSRMLS_CC);
				return;
			}
			zend_hash_move_forward_ex(&intern->storage, &intern->pos);
		}
	}

	spl_object_storage_attach(intern, getThis(), iterator, info TSRMLS_CC);
}