coolkey_get_object(sc_card_t *card, unsigned long object_id, sc_cardctl_coolkey_object_t **obj) {
	sc_cardctl_coolkey_find_object_t fobj;
	int r;

	fobj.type = SC_CARDCTL_COOLKEY_FIND_BY_ID;
	fobj.find_id = object_id;
	fobj.obj = NULL;
	r = sc_card_ctl(card, SC_CARDCTL_COOLKEY_FIND_OBJECT, &fobj);
	if (r < 0) {
		return r;
	}
	*obj = fobj.obj;
	return SC_SUCCESS;
}