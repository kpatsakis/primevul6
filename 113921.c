coolkey_get_attribute_ulong(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_ATTRIBUTE_TYPE type, CK_ULONG *value)
{
	const u8 *val;
	size_t val_len;
	u8 data_type;
	int r;

	r  = coolkey_get_attribute(card, obj, type, &val, &val_len, &data_type);
	if (r < 0) {
		return r;
	}
	if ((data_type != SC_CARDCTL_COOLKEY_ATTR_TYPE_ULONG) &&
	    (val_len != sizeof(CK_ULONG))) {
		return SC_ERROR_CORRUPTED_DATA;
	}
	*value = bebytes2ulong(val);
	return SC_SUCCESS;
}