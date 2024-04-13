coolkey_get_attribute(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_ATTRIBUTE_TYPE type, const u8 **val, size_t *val_len, u8 *data_type) {
	sc_cardctl_coolkey_attribute_t attribute;
	int r;

	attribute.object = obj;
	attribute.attribute_type = type;

	r = sc_card_ctl(card, SC_CARDCTL_COOLKEY_GET_ATTRIBUTE, &attribute);
	if (r < 0) {
		return r;
	}
	*val = attribute.attribute_value;
	*val_len = attribute.attribute_length;
	if (data_type) {
		*data_type = attribute.attribute_data_type;
	}
	return SC_SUCCESS;
}