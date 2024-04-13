static int spl_object_storage_compare_objects(zval *o1, zval *o2 TSRMLS_DC) /* {{{ */
{
	zend_object *zo1 = (zend_object *)zend_object_store_get_object(o1 TSRMLS_CC);
	zend_object *zo2 = (zend_object *)zend_object_store_get_object(o2 TSRMLS_CC);

	if (zo1->ce != spl_ce_SplObjectStorage || zo2->ce != spl_ce_SplObjectStorage) {
		return 1;
	}

	return zend_hash_compare(&((spl_SplObjectStorage *)zo1)->storage, &((spl_SplObjectStorage *)zo2)->storage, (compare_func_t) spl_object_storage_compare_info, 0 TSRMLS_CC);
}