coolkey_get_access(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, unsigned int *access_ptr)
{
	return coolkey_set_bool_flags(card, obj, access_ptr, access_table, access_table_size);
}