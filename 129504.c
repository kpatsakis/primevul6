void spl_object_storage_addall(spl_SplObjectStorage *intern, zval *this, spl_SplObjectStorage *other TSRMLS_DC) { /* {{{ */
	HashPosition pos;
	spl_SplObjectStorageElement *element;

	zend_hash_internal_pointer_reset_ex(&other->storage, &pos);
	while (zend_hash_get_current_data_ex(&other->storage, (void **)&element, &pos) == SUCCESS) {
		spl_object_storage_attach(intern, this, element->obj, element->inf TSRMLS_CC);
		zend_hash_move_forward_ex(&other->storage, &pos);
	}

	zend_hash_internal_pointer_reset_ex(&intern->storage, &intern->pos);
	intern->index = 0;
} /* }}} */