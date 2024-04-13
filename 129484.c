spl_SplObjectStorageElement* spl_object_storage_get(spl_SplObjectStorage *intern, char *hash, int hash_len TSRMLS_DC) /* {{{ */
{
	spl_SplObjectStorageElement *element;
	if (zend_hash_find(&intern->storage, hash, hash_len, (void**)&element) == SUCCESS) {
		return element;
	} else {
		return NULL;
	}
} /* }}} */