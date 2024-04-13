ca_getkey(struct privsep *ps, struct iked_id *key, enum imsg_type type)
{
	struct iked	*env = ps->ps_env;
	struct ca_store	*store = env->sc_priv;
	struct iked_id	*id;
	const char	*name;

	if (store == NULL)
		fatalx("%s: invalid store", __func__);

	if (type == IMSG_PRIVKEY) {
		name = "private";
		id = &store->ca_privkey;

		store->ca_privkey_method = ca_privkey_to_method(key);
		if (store->ca_privkey_method == IKEV2_AUTH_NONE)
			fatalx("ca: failed to get auth method for privkey");
	} else if (type == IMSG_PUBKEY) {
		name = "public";
		id = &store->ca_pubkey;
	} else
		fatalx("%s: invalid type %d", __func__, type);

	log_debug("%s: received %s key type %s length %zd", __func__,
	    name, print_map(key->id_type, ikev2_cert_map),
	    ibuf_length(key->id_buf));

	/* clear old key and copy new one */
	ibuf_release(id->id_buf);
	memcpy(id, key, sizeof(*id));
}