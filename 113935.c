coolkey_get_public_key(sc_pkcs15_card_t *p15card, sc_cardctl_coolkey_object_t *obj, CK_KEY_TYPE key_type)
{
	sc_pkcs15_pubkey_t *key;

	key = coolkey_make_public_key(p15card->card, obj, key_type);
	if (key) {
		return key;
	}
	return coolkey_get_public_key_from_certificate(p15card, obj);
}