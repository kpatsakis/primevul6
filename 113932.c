coolkey_set_bool_flags(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, unsigned int *flags_ptr, struct coolkey_attr_flags *table, int table_size)
{
	unsigned int flags = 0;
	int i;

	for (i=0; i< table_size; i++) {
		if (coolkey_get_attribute_boolean(card, obj, table[i].attribute_type)) {
			flags |= table[i].pkcs15_flags;
		}
	}
	*flags_ptr = flags;
	return SC_SUCCESS;
}