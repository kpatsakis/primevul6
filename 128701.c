static int php_zip_property_reader(ze_zip_object *obj, zip_prop_handler *hnd, zval **retval, int newzval TSRMLS_DC) /* {{{ */
{
	const char *retchar = NULL;
	int retint = 0;
	int len = 0;

	if (obj && obj->za != NULL) {
		if (hnd->read_const_char_func) {
			retchar = hnd->read_const_char_func(obj->za, &len TSRMLS_CC);
		} else {
			if (hnd->read_int_func) {
				retint = hnd->read_int_func(obj->za TSRMLS_CC);
				if (retint == -1) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal zip error returned");
					return FAILURE;
				}
			} else {
				if (hnd->read_const_char_from_obj_func) {
					retchar = hnd->read_const_char_from_obj_func(obj TSRMLS_CC);
					len = strlen(retchar);
				}
			}
		}
	}

	if (newzval) {
		ALLOC_ZVAL(*retval);
	}

	switch (hnd->type) {
		case IS_STRING:
			if (retchar) {
				ZVAL_STRINGL(*retval, (char *) retchar, len, 1);
			} else {
				ZVAL_EMPTY_STRING(*retval);
			}
			break;
		case IS_BOOL:
			ZVAL_BOOL(*retval, (long)retint);
			break;
		case IS_LONG:
			ZVAL_LONG(*retval, (long)retint);
			break;
		default:
			ZVAL_NULL(*retval);
	}

	return SUCCESS;
}