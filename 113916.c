coolkey_get_attribute_boolean(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_ATTRIBUTE_TYPE attr_type)
{
	int r;
	const u8 *val;
	size_t val_len;

	r = coolkey_get_attribute(card, obj, attr_type, &val, &val_len, NULL);
	if (r < 0) {
		/* attribute not valid for this object, set boolean to false */
		return 0;
	}
	if ((val_len == 1) && (*val == 1)) {
		return 1;
	}
	return 0;
}