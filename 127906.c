PHP_MINIT_FUNCTION(exif)
{
	REGISTER_INI_ENTRIES();
	if (zend_hash_str_exists(&module_registry, "mbstring", sizeof("mbstring")-1)) {
		REGISTER_LONG_CONSTANT("EXIF_USE_MBSTRING", 1, CONST_CS | CONST_PERSISTENT);
	} else {
		REGISTER_LONG_CONSTANT("EXIF_USE_MBSTRING", 0, CONST_CS | CONST_PERSISTENT);
	}
	return SUCCESS;
}