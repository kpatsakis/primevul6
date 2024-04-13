static zend_object_value spl_SplObjectStorage_new(zend_class_entry *class_type TSRMLS_DC)
{
	spl_SplObjectStorage *tmp;
	return spl_object_storage_new_ex(class_type, &tmp, NULL TSRMLS_CC);
}