coolkey_make_public_key(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, CK_KEY_TYPE key_type)
{
	sc_pkcs15_pubkey_t *key;
	int r;

	key = calloc(1, sizeof(struct sc_pkcs15_pubkey));
	if (!key)
		return NULL;
	switch (key_type) {
	case CKK_RSA:
		key->algorithm = SC_ALGORITHM_RSA;
		r = coolkey_get_attribute_lv(card, obj, CKA_MODULUS, &key->u.rsa.modulus);
		if (r != SC_SUCCESS) {
			goto fail;
		}
		r = coolkey_get_attribute_lv(card, obj, CKA_PUBLIC_EXPONENT, &key->u.rsa.exponent);
		if (r != SC_SUCCESS) {
			goto fail;
		}
		break;
	case CKK_EC:
		key->algorithm = SC_ALGORITHM_EC;
		r = coolkey_get_attribute_bytes_alloc(card, obj, CKA_EC_POINT, &key->u.ec.ecpointQ.value, &key->u.ec.ecpointQ.len);
	    if(r < 0) {
			goto fail;
		}
		r = coolkey_get_attribute_bytes_alloc(card, obj, CKA_EC_PARAMS,
				&key->u.ec.params.der.value, &key->u.ec.params.der.len);
		if (r < 0) {
			goto fail;
		}
		r = sc_pkcs15_fix_ec_parameters(card->ctx, &key->u.ec.params);
		if (r < 0) {
			goto fail;
		}
		break;
	}
	return key;
fail:
	sc_pkcs15_free_pubkey(key);

	/* now parse the DER cert */
	return NULL;
}