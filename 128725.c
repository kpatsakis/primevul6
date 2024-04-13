static void php_zip_entry_get_info(INTERNAL_FUNCTION_PARAMETERS, int opt) /* {{{ */
{
	zval * zip_entry;
	zip_read_rsrc * zr_rsrc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zip_entry) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(zr_rsrc, zip_read_rsrc *, &zip_entry, -1, le_zip_entry_name, le_zip_entry);

	if (!zr_rsrc->zf) {
		RETURN_FALSE;
	}

	switch (opt) {
		case 0:
			RETURN_STRING((char *)zr_rsrc->sb.name, 1);
			break;
		case 1:
			RETURN_LONG((long) (zr_rsrc->sb.comp_size));
			break;
		case 2:
			RETURN_LONG((long) (zr_rsrc->sb.size));
			break;
		case 3:
			switch (zr_rsrc->sb.comp_method) {
				case 0:
					RETURN_STRING("stored", 1);
					break;
				case 1:
					RETURN_STRING("shrunk", 1);
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					RETURN_STRING("reduced", 1);
					break;
				case 6:
					RETURN_STRING("imploded", 1);
					break;
				case 7:
					RETURN_STRING("tokenized", 1);
					break;
				case 8:
					RETURN_STRING("deflated", 1);
					break;
				case 9:
					RETURN_STRING("deflatedX", 1);
					break;
				case 10:
					RETURN_STRING("implodedX", 1);
					break;
				default:
					RETURN_FALSE;
			}
			RETURN_LONG((long) (zr_rsrc->sb.comp_method));
			break;
	}

}