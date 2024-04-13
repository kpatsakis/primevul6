coolkey_find_matching_cert(sc_card_t *card, sc_cardctl_coolkey_object_t *in_obj, sc_cardctl_coolkey_object_t **cert_obj) {
	sc_cardctl_coolkey_find_object_t fobj;
	sc_cardctl_coolkey_attribute_t template[2];
	u8 obj_class[4];
	int r;

	/* we are searching for certs .. */
	template[0].attribute_type = CKA_CLASS;
	template[0].attribute_data_type = SC_CARDCTL_COOLKEY_ATTR_TYPE_ULONG;
	template[0].attribute_length = sizeof(obj_class);
	template[0].attribute_value = obj_class;
	ulong2bebytes(obj_class, CKO_CERTIFICATE);

	/* fetch the current object's CKA_ID */
	template[1].attribute_type = CKA_ID;
	template[1].object = in_obj;
	r = sc_card_ctl(card, SC_CARDCTL_COOLKEY_GET_ATTRIBUTE, &template[1]);
	if (r < 0) {
		return r;
	}
	template[0].object = NULL; /*paranoia */
	template[1].object = NULL; /*paranoia */

	/* now find the cert that has the ID */
	fobj.type = SC_CARDCTL_COOLKEY_FIND_BY_TEMPLATE;
	fobj.obj = NULL;
	fobj.coolkey_template = &template[0];
	fobj.template_count=2;
	r = sc_card_ctl(card, SC_CARDCTL_COOLKEY_FIND_OBJECT, &fobj);
	if (r < 0) {
		return r;
	}
	*cert_obj = fobj.obj;
	return SC_SUCCESS;
}