coolkey_get_attribute_lv(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_ATTRIBUTE_TYPE type, void *ptr)
{
	struct sc_pkcs15_data *item = (struct sc_pkcs15_data *)ptr;
	return coolkey_get_attribute_bytes_alloc(card, obj, type, &item->data, &item->data_len);
}