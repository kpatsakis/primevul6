static inline int unserialize_allowed_class(zend_string *class_name, HashTable *classes)
{
	zend_string *lcname;
	int res;
	ALLOCA_FLAG(use_heap)

	if(classes == NULL) {
		return 1;
	}
	if(!zend_hash_num_elements(classes)) {
		return 0;
	}

	ZSTR_ALLOCA_ALLOC(lcname, ZSTR_LEN(class_name), use_heap);
	zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(class_name), ZSTR_LEN(class_name));
	res = zend_hash_exists(classes, lcname);
	ZSTR_ALLOCA_FREE(lcname, use_heap);
	return res;
}