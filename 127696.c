PHPAPI void var_push_dtor(php_unserialize_data_t *var_hashx, zval *rval)
{
	zval *tmp_var = var_tmp_var(var_hashx);
    if (!tmp_var) {
        return;
    }
	ZVAL_COPY(tmp_var, rval);
}