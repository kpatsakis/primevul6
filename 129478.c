SPL_METHOD(MultipleIterator, __construct)
{
	spl_SplObjectStorage   *intern;
	long                    flags = MIT_NEED_ALL|MIT_KEYS_NUMERIC;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_InvalidArgumentException, &error_handling TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &flags) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);
	intern->flags = flags;
	zend_restore_error_handling(&error_handling TSRMLS_CC);
}