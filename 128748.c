static void php_zip_object_free_storage(void *object TSRMLS_DC) /* {{{ */
{
	ze_zip_object * intern = (ze_zip_object *) object;
	int i;

	if (!intern) {
		return;
	}
	if (intern->za) {
		if (zip_close(intern->za) != 0) {
			_zip_free(intern->za);
		}
		intern->za = NULL;
	}

	if (intern->buffers_cnt>0) {
		for (i=0; i<intern->buffers_cnt; i++) {
			efree(intern->buffers[i]);
		}
		efree(intern->buffers);
	}

	intern->za = NULL;

#if (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION == 1 && PHP_RELEASE_VERSION > 2) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 1) || (PHP_MAJOR_VERSION > 5)
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
#else
	if (intern->zo.guards) {
		zend_hash_destroy(intern->zo.guards);
		FREE_HASHTABLE(intern->zo.guards);
	}

	if (intern->zo.properties) {
		zend_hash_destroy(intern->zo.properties);
		FREE_HASHTABLE(intern->zo.properties);
	}
#endif

	if (intern->filename) {
		efree(intern->filename);
	}
	efree(intern);
}