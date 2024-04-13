static int spl_object_storage_compare_info(spl_SplObjectStorageElement *e1, spl_SplObjectStorageElement *e2 TSRMLS_DC) /* {{{ */
{
	zval result;

	if (compare_function(&result, e1->inf, e2->inf TSRMLS_CC) == FAILURE) {
		return 1;
	}

	return Z_LVAL(result);
}