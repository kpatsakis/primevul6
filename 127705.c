static zend_always_inline int process_nested_data(UNSERIALIZE_PARAMETER, HashTable *ht, zend_long elements, int objprops)
{
	while (elements-- > 0) {
		zval key, *data, d, *old_data;
		zend_ulong idx;

		ZVAL_UNDEF(&key);

		if (!php_var_unserialize_internal(&key, p, max, NULL, classes)) {
			zval_dtor(&key);
			return 0;
		}

		data = NULL;
		ZVAL_UNDEF(&d);

		if (!objprops) {
			if (Z_TYPE(key) == IS_LONG) {
				idx = Z_LVAL(key);
numeric_key:
				if (UNEXPECTED((old_data = zend_hash_index_find(ht, idx)) != NULL)) {
					//??? update hash
					var_push_dtor(var_hash, old_data);
					data = zend_hash_index_update(ht, idx, &d);
				} else {
					data = zend_hash_index_add_new(ht, idx, &d);
				}
			} else if (Z_TYPE(key) == IS_STRING) {
				if (UNEXPECTED(ZEND_HANDLE_NUMERIC(Z_STR(key), idx))) {
					goto numeric_key;
				}
				if (UNEXPECTED((old_data = zend_hash_find(ht, Z_STR(key))) != NULL)) {
					//??? update hash
					var_push_dtor(var_hash, old_data);
					data = zend_hash_update(ht, Z_STR(key), &d);
				} else {
					data = zend_hash_add_new(ht, Z_STR(key), &d);
				}
			} else {
				zval_dtor(&key);
				return 0;
			}
		} else {
			if (EXPECTED(Z_TYPE(key) == IS_STRING)) {
string_key:
				if ((old_data = zend_hash_find(ht, Z_STR(key))) != NULL) {
					if (Z_TYPE_P(old_data) == IS_INDIRECT) {
						old_data = Z_INDIRECT_P(old_data);
					}
					var_push_dtor(var_hash, old_data);
					data = zend_hash_update_ind(ht, Z_STR(key), &d);
				} else {
					data = zend_hash_add_new(ht, Z_STR(key), &d);
				}
			} else if (Z_TYPE(key) == IS_LONG) {
				/* object properties should include no integers */
				convert_to_string(&key);
				goto string_key;
			} else {
				zval_dtor(&key);
				return 0;
			}
		}

		if (!php_var_unserialize_internal(data, p, max, var_hash, classes)) {
			zval_dtor(&key);
			return 0;
		}

		if (UNEXPECTED(Z_ISUNDEF_P(data))) {
			if (Z_TYPE(key) == IS_LONG) {
				zend_hash_index_del(ht, Z_LVAL(key));
			} else {
				zend_hash_del_ind(ht, Z_STR(key));
			}
		} else {
			var_push_dtor(var_hash, data);
		}

		zval_dtor(&key);

		if (elements && *(*p-1) != ';' && *(*p-1) != '}') {
			(*p)--;
			return 0;
		}
	}

	return 1;
}