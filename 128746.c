static int php_zip_has_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ze_zip_object *obj;
	zval tmp_member;
	zip_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret, retval = 0;

	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
		key = NULL;
	}

	ret = FAILURE;
	obj = (ze_zip_object *)zend_objects_get_address(object TSRMLS_CC);

	if (obj->prop_handler != NULL) {
		if (key) {
			ret = zend_hash_quick_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, key->hash_value, (void **) &hnd);
		} else {
			ret = zend_hash_find(obj->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
		}
	}

	if (ret == SUCCESS) {
		zval *tmp;
		ALLOC_INIT_ZVAL(tmp);

		if (type == 2) {
			retval = 1;
		} else if (php_zip_property_reader(obj, hnd, &tmp, 0 TSRMLS_CC) == SUCCESS) {
			Z_SET_REFCOUNT_P(tmp, 1);
			Z_UNSET_ISREF_P(tmp);
			if (type == 1) {
				retval = zend_is_true(tmp);
			} else if (type == 0) {
				retval = (Z_TYPE_P(tmp) != IS_NULL);
			}
		}

		zval_ptr_dtor(&tmp);
	} else {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->has_property(object, member, type, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}