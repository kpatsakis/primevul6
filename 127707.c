PHPAPI int php_var_unserialize(zval *rval, const unsigned char **p, const unsigned char *max, php_unserialize_data_t *var_hash)
{
	HashTable *classes = NULL;
	return php_var_unserialize_ex(UNSERIALIZE_PASSTHRU);
}