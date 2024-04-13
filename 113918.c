coolkey_get_attribute_bytes_alloc(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_ATTRIBUTE_TYPE type, u8 **data, size_t *data_len)
{
	const u8 *val;
	size_t val_len;
	int r;

	r = coolkey_get_attribute(card, obj, type, &val, &val_len, NULL);
	if (r < 0) {
		return r;
	}
	*data = malloc(val_len);
	if (*data == NULL) {
		return SC_ERROR_OUT_OF_MEMORY;
	}
	memcpy(*data, val, val_len);
	*data_len = val_len;
	return SC_SUCCESS;
}