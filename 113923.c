coolkey_get_id(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, struct sc_pkcs15_id *id)
{
	return coolkey_get_attribute_bytes(card, obj, CKA_ID, id->value , &id->len, sizeof(id->value));
}