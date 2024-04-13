static void php_zip_free_dir(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	zip_rsrc * zip_int = (zip_rsrc *) rsrc->ptr;

	if (zip_int) {
		if (zip_int->za) {
			if (zip_close(zip_int->za) != 0) {
				_zip_free(zip_int->za);
			}
			zip_int->za = NULL;
		}

		efree(rsrc->ptr);

		rsrc->ptr = NULL;
	}
}