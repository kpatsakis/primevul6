int spl_object_storage_contains(spl_SplObjectStorage *intern, zval *this, zval *obj TSRMLS_DC) /* {{{ */
{
	int hash_len, found;
	char *hash = spl_object_storage_get_hash(intern, this, obj, &hash_len TSRMLS_CC);
	if (!hash) {
		return 0;
	}

	found = zend_hash_exists(&intern->storage, hash, hash_len);
	spl_object_storage_free_hash(intern, hash);
	return found;
} /* }}} */