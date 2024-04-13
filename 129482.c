static char *spl_object_storage_get_hash(spl_SplObjectStorage *intern, zval *this,  zval *obj, int *hash_len_ptr TSRMLS_DC) {
	if (intern->fptr_get_hash) {
		zval *rv;
		zend_call_method_with_1_params(&this, intern->std.ce, &intern->fptr_get_hash, "getHash", &rv, obj);
		if (rv) {
			if (Z_TYPE_P(rv) == IS_STRING) {
				int hash_len = Z_STRLEN_P(rv);
				char *hash = emalloc((hash_len+1)*sizeof(char));
				strncpy(hash, Z_STRVAL_P(rv), hash_len);
				hash[hash_len] = 0;

				zval_ptr_dtor(&rv);
				if (hash_len_ptr) {
					*hash_len_ptr = hash_len;
				}
				return hash;
			} else {
				zend_throw_exception(spl_ce_RuntimeException, "Hash needs to be a string", 0 TSRMLS_CC);

				zval_ptr_dtor(&rv);
				return NULL;
			}
		} else {
			return NULL;
		}
	} else {
		int hash_len = sizeof(zend_object_value);

#if HAVE_PACKED_OBJECT_VALUE

		if (hash_len_ptr) {
			*hash_len_ptr = hash_len;
		}

		return (char*)&Z_OBJVAL_P(obj);
#else
		char *hash = emalloc(hash_len + 1);

		zend_object_value zvalue;
		memset(&zvalue, 0, sizeof(zend_object_value));
		zvalue.handle = Z_OBJ_HANDLE_P(obj);
		zvalue.handlers = Z_OBJ_HT_P(obj);

		memcpy(hash, (char *)&zvalue, hash_len);
		hash[hash_len] = 0;

		if (hash_len_ptr) {
			*hash_len_ptr = hash_len;
		}

		return hash;
#endif
	}
}