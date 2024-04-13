SPL_METHOD(MultipleIterator, setFlags)
{
	spl_SplObjectStorage *intern;
	intern = (spl_SplObjectStorage*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &intern->flags) == FAILURE) {
		return;
	}
}