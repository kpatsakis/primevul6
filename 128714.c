static void php_zip_free_entry(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	zip_read_rsrc *zr_rsrc = (zip_read_rsrc *) rsrc->ptr;

	if (zr_rsrc) {
		if (zr_rsrc->zf) {
			if (zr_rsrc->zf->za) {
				zip_fclose(zr_rsrc->zf);
			} else {
				if (zr_rsrc->zf->src)
					zip_source_free(zr_rsrc->zf->src);
				free(zr_rsrc->zf);
			}
			zr_rsrc->zf = NULL;
		}
		efree(zr_rsrc);
		rsrc->ptr = NULL;
	}
}