coolkey_get_certificate(sc_card_t *card, sc_cardctl_coolkey_object_t *obj, struct sc_pkcs15_der *cert)
{
	sc_cardctl_coolkey_object_t *cert_obj;
	unsigned long object_id;
	int r;

	cert_obj = obj;
	if (coolkey_get_object_type(obj->id) != COOLKEY_ID_CERT) {
		r = coolkey_find_matching_cert(card, obj, &cert_obj);
		if (r < 0) {
			return r;
		}
	}
	r = coolkey_get_attribute_lv(card, cert_obj, CKA_VALUE, cert);
	if (r == SC_ERROR_DATA_OBJECT_NOT_FOUND) {
		object_id = coolkey_make_new_id(cert_obj->id, COOLKEY_ID_CERT_DATA);
		r = coolkey_get_object(card, object_id, &cert_obj);
		if (r < 0) {
			return r;
		}
		/* fill in cert data */
		cert->value = malloc(cert_obj->length);
		if (cert->value == NULL) {
			return SC_ERROR_OUT_OF_MEMORY;
		}
		memcpy(cert->value, cert_obj->data, cert_obj->length);
		cert->len = cert_obj->length;
		return SC_SUCCESS;
	}
	return r;
}