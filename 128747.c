static zval* php_zip_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC) /* {{{ */
{
	ze_zip_object *obj;
	zval tmp_member;
	zval *retval;
	zip_prop_handler *hnd;
	zend_object_handlers *std_hnd;
	int ret;

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
		ret = php_zip_property_reader(obj, hnd, &retval, 1 TSRMLS_CC);
		if (ret == SUCCESS) {
			/* ensure we're creating a temporary variable */
			Z_SET_REFCOUNT_P(retval, 0);
		} else {
			retval = EG(uninitialized_zval_ptr);
		}
	} else {
		std_hnd = zend_get_std_object_handlers();
		retval = std_hnd->read_property(object, member, type, key TSRMLS_CC);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}
	return retval;
}