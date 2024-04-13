coolkey_get_flags(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, unsigned int *flag_ptr)
{
	return coolkey_set_bool_flags(card, obj, flag_ptr, flag_table, flag_table_size);
}