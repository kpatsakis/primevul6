coolkey_get_usage(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, unsigned int *usage_ptr)
{
	return coolkey_set_bool_flags(card, obj, usage_ptr, usage_table, usage_table_size);
}