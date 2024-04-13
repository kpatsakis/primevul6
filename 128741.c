static HashTable *php_zip_get_properties(zval *object TSRMLS_DC)/* {{{ */
{
	ze_zip_object *obj;
	zip_prop_handler *hnd;
	HashTable *props;
	zval *val;
	int ret;
	char *key;
	uint key_len;
	HashPosition pos;
	ulong num_key;

	obj = (ze_zip_object *)zend_objects_get_address(object TSRMLS_CC);
	props = zend_std_get_properties(object TSRMLS_CC);

	if (obj->prop_handler == NULL) {
		return NULL;
	}
	zend_hash_internal_pointer_reset_ex(obj->prop_handler, &pos);

	while (zend_hash_get_current_data_ex(obj->prop_handler, (void**)&hnd, &pos) == SUCCESS) {
		zend_hash_get_current_key_ex(obj->prop_handler, &key, &key_len, &num_key, 0, &pos);
		MAKE_STD_ZVAL(val);
		ret = php_zip_property_reader(obj, hnd, &val, 0 TSRMLS_CC);
		if (ret != SUCCESS) {
			val = EG(uninitialized_zval_ptr);
		}
		zend_hash_update(props, key, key_len, (void *)&val, sizeof(zval *), NULL);
		zend_hash_move_forward_ex(obj->prop_handler, &pos);
	}
	return props;
}