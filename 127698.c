static zval *var_access(php_unserialize_data_t *var_hashx, zend_long id)
{
	var_entries *var_hash = (*var_hashx)->first;
#if VAR_ENTRIES_DBG
	fprintf(stderr, "var_access(%ld): %ld\n", var_hash?var_hash->used_slots:-1L, id);
#endif

	while (id >= VAR_ENTRIES_MAX && var_hash && var_hash->used_slots == VAR_ENTRIES_MAX) {
		var_hash = var_hash->next;
		id -= VAR_ENTRIES_MAX;
	}

	if (!var_hash) return NULL;

	if (id < 0 || id >= var_hash->used_slots) return NULL;

	return var_hash->data[id];
}