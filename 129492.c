static void spl_object_storage_dtor(spl_SplObjectStorageElement *element) /* {{{ */
{
	zval_ptr_dtor(&element->obj);
	zval_ptr_dtor(&element->inf);
} /* }}} */