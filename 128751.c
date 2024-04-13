static zend_object_value php_zip_object_new(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	ze_zip_object *intern;
	zend_object_value retval;

	intern = emalloc(sizeof(ze_zip_object));
	memset(&intern->zo, 0, sizeof(zend_object));

	intern->za = NULL;
	intern->buffers = NULL;
	intern->filename = NULL;
	intern->buffers_cnt = 0;
	intern->prop_handler = &zip_prop_handlers;

#if ((PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1 && PHP_RELEASE_VERSION > 2))
	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
#else
	ALLOC_HASHTABLE(intern->zo.properties);
  	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	intern->zo.ce = class_type;
#endif

	object_properties_init(&intern->zo, class_type);

	retval.handle = zend_objects_store_put(intern,
						NULL,
						(zend_objects_free_object_storage_t) php_zip_object_free_storage,
						NULL TSRMLS_CC);

	retval.handlers = (zend_object_handlers *) & zip_object_handlers;

	return retval;
}